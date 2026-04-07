#include "mc-garbage-collector.hpp"

#include <atomic>
#include <cassert>
#include <latch>
#include <cstring>

#include "../common/cfg/heap-cfg.hpp"
#include "../common/stack/indexed-stack.hpp"

mc_garbage_collector::mc_garbage_collector(size_t thread_count) : garbage_collector{ thread_count } {}

void mc_garbage_collector::collect(root_set_table& root_set, heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept {
    mark(root_set);
    compute_forwarding_addresses(heap_memory);
    update_roots(root_set);
    update_heap_refs(heap_memory);
    compact(heap_memory, free_memory_table);
}

void mc_garbage_collector::mark_object(header* hdr) noexcept {
    assert(hdr != nullptr);
    
    indexed_stack<header*> refs{64};
    refs.push(hdr);
    
    while(!refs.empty()) {
        header* current{ refs.peek() };
        refs.pop();
        
        if(!current->try_mark()) continue;

        current->trace_refs([&refs](header** ref_slot) -> void {
            if(header* ref{ *ref_slot }) {
                refs.push(ref);
            }
        });
    }
}

void mc_garbage_collector::visit(thread_local_stack& stack){
    stack.for_each([this](header*& root) -> void {
        mark_object(root);
    });
}

void mc_garbage_collector::visit(shared_global_space& global){
    global.for_each([this](header*& obj) -> void {
        mark_object(obj);
    });
}

void mc_garbage_collector::visit(shared_register_space& reg){
    reg.for_each([this](header*& obj) -> void {
        mark_object(obj);
    });
}

void mc_garbage_collector::forward_object(header* hdr) noexcept {
    assert(hdr != nullptr && hdr->forwarding_address != nullptr);

    hdr->trace_refs([](header** ref_slot) -> void {
        if(*ref_slot && (*ref_slot)->forwarding_address){
            *ref_slot = (*ref_slot)->forwarding_address;
        }
    });
}

void mc_garbage_collector::forward(thread_local_stack& stack){
    stack.for_each([](header*& root) -> void {
        root = root->forwarding_address;
    });
}

void mc_garbage_collector::forward(shared_global_space& global){
    global.for_each([](header*& obj) -> void {
        if(obj->forwarding_address){
            obj = obj->forwarding_address;
        }
    });
}

void mc_garbage_collector::forward(shared_register_space& reg){
    reg.for_each([](header*& obj) -> void {
        if(obj->forwarding_address){
            obj = obj->forwarding_address;
        }
    });
}

void mc_garbage_collector::mark(root_set_table& root_set) noexcept {
    const size_t total{ root_set.get_root_count() };
    if(total == 0) return;

    std::latch completion_latch{ static_cast<std::ptrdiff_t>(total) };

    auto& roots_table{ root_set.get_roots() };
    auto** buckets{ roots_table.get_buckets() };
    const size_t capacity{ roots_table.get_capacity() };

    for(size_t i{0}; i < capacity; ++i) {
        for(auto* root{ buckets[i] }; root; root = root->next){
            auto* root_obj{ root->value };
            gc_thread_pool.enqueue(
                [this, root_obj, &completion_latch] -> void {
                    if(root_obj){
                        root_obj->accept(*this);
                    }
                    completion_latch.count_down();
                }
            );
        }
    }

    completion_latch.wait();
}

void mc_garbage_collector::compute_forwarding_addresses_segment(segment& seg) noexcept {
    uint8_t* free_ptr{ seg.segment_memory };
    uint8_t* scan_ptr{ seg.segment_memory };
    const uint8_t* end_ptr{ seg.segment_memory + cfg::heap::SEGMENT_SIZE };

    while(scan_ptr + sizeof(header) <= end_ptr) {
        header* hdr{ reinterpret_cast<header*>(scan_ptr) };
        const size_t object_size{ sizeof(header) + static_cast<size_t>(hdr->size) };

        if(hdr->is_marked()) {
            hdr->forwarding_address = reinterpret_cast<header*>(free_ptr);
            free_ptr += object_size;
        }
        else {
            hdr->forwarding_address = nullptr;
        }

        scan_ptr += object_size;
    }
}

void mc_garbage_collector::compute_forwarding_addresses(heap& heap_memory) noexcept {
    if constexpr (cfg::heap::TOTAL_SEGMENTS == 0) return;

    std::latch completion_latch{ cfg::heap::TOTAL_SEGMENTS };

    auto enqueue_segment_forward{ 
        [this, &completion_latch](segment& segment) -> void {
            gc_thread_pool.enqueue(
                [this, seg_ptr=&segment, &completion_latch] -> void {
                    compute_forwarding_addresses_segment(*seg_ptr);
                    completion_latch.count_down();
                }
            );
        }
    };

    for(size_t i{0}; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_small_object_segment(i));
    }

    for(size_t i{0}; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_medium_object_segment(i));
    }

    for(size_t i{0}; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_large_object_segment(i));
    }

    completion_latch.wait();
}

void mc_garbage_collector::update_roots(root_set_table& root_set) noexcept {
    const size_t total{ root_set.get_root_count() };
    if(total == 0) return;

    std::latch completion_latch { static_cast<std::ptrdiff_t>(total) };

    auto& roots_table{ root_set.get_roots() };
    auto** buckets{ roots_table.get_buckets() };
    const size_t capacity{ roots_table.get_capacity() };

    for(size_t i{0}; i < capacity; ++i) {
        for(auto* root{ buckets[i] }; root; root = root->next) {
            auto* root_obj{ root->value };
            gc_thread_pool.enqueue(
                [this, root_obj, &completion_latch] -> void {
                    if(root_obj) {
                        root_obj->accept_forward(*this);
                    }
                    completion_latch.count_down();
                }
            );
        }
    }

    completion_latch.wait();
}

void mc_garbage_collector::update_segment_refs(segment& seg) noexcept {
    uint8_t* scan_ptr{ seg.segment_memory };
    const uint8_t* end_ptr{ seg.segment_memory + cfg::heap::SEGMENT_SIZE };

    while(scan_ptr + sizeof(header) <= end_ptr) {
        header* hdr{ reinterpret_cast<header*>(scan_ptr) };

        if(hdr->forwarding_address){
            forward_object(hdr);
        }

        scan_ptr += sizeof(header) + static_cast<size_t>(hdr->size);
    }
}

void mc_garbage_collector::update_heap_refs(heap& heap_memory) noexcept {
    if constexpr (cfg::heap::TOTAL_SEGMENTS == 0) return;

    std::latch completion_latch{ cfg::heap::TOTAL_SEGMENTS };

    auto enqueue_segment_forward{ 
        [this, &completion_latch](segment& segment) -> void {
            gc_thread_pool.enqueue(
                [this, seg_ptr = &segment, &completion_latch] -> void {
                    update_segment_refs(*seg_ptr);
                    completion_latch.count_down();
                }
            );
        }
    };

    for(size_t i{0}; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_small_object_segment(i));
    }

    for(size_t i{0}; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_medium_object_segment(i));
    }

    for(size_t i{0}; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_large_object_segment(i));
    }

    completion_latch.wait();
}

void mc_garbage_collector::compact_segment(segment& seg, segment_info* seg_info) noexcept {
    uint8_t* scan_ptr{ seg.segment_memory };
    const uint8_t* end_ptr{ seg.segment_memory + cfg::heap::SEGMENT_SIZE };
    uint32_t used_bytes{0};

    while(scan_ptr + sizeof(header) <= end_ptr) {
        header* hdr{ reinterpret_cast<header*>(scan_ptr) };
        const size_t object_size{ sizeof(header) + static_cast<size_t>(hdr->size) };

        if(hdr->forwarding_address) {
            header* dest{ hdr->forwarding_address };
            if(dest != hdr){
                std::memmove(dest, hdr, object_size);
            }
            dest->forwarding_address = nullptr;
            dest->set_marked(false);
            used_bytes += object_size;
        }

        scan_ptr += object_size;
    }

    const uint32_t free_bytes{ cfg::heap::SEGMENT_SIZE - used_bytes };
    if(static_cast<size_t>(free_bytes) > sizeof(header)){
        header* free_hdr{ reinterpret_cast<header*>(seg.segment_memory + used_bytes) };
        free_hdr->size = free_bytes - sizeof(header);
        free_hdr->set_free(true);
        free_hdr->next = nullptr;

        seg_info->free_list_head = free_hdr;
        seg_info->free_bytes.store(free_hdr->size, std::memory_order_relaxed);
    }

}

void mc_garbage_collector::compact(heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept {
    if constexpr (cfg::heap::TOTAL_SEGMENTS == 0) return;

    std::latch completion_latch{ cfg::heap::TOTAL_SEGMENTS };

    auto enqueue_segment_compact = 
        [this, &completion_latch](segment& segment, segment_info* seg_info) -> void {
            gc_thread_pool.enqueue(
                [this, seg = &segment, seg_info, &completion_latch] -> void {
                    compact_segment(*seg, seg_info);
                    completion_latch.count_down();
                }
            );
    };

    size_t absolute_idx{0};
    for(size_t i{0}; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_compact(
            heap_memory.get_small_object_segment(i), 
            free_memory_table.get_segment_info(absolute_idx++)
        );
    }

    for(size_t i{0}; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_compact(
            heap_memory.get_medium_object_segment(i), 
            free_memory_table.get_segment_info(absolute_idx++)
        );
    }

    for(size_t i{0}; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_compact(
            heap_memory.get_large_object_segment(i), 
            free_memory_table.get_segment_info(absolute_idx++)
        );
    }

    completion_latch.wait();
}