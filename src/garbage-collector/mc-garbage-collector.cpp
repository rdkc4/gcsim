#include "mc-garbage-collector.hpp"

#include <cassert>
#include <latch>
#include <cstring>

#include "../common/cfg/heap-cfg.hpp"
#include "../common/indexed-stack/indexed-stack.hpp"

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
        header* current = refs.peek();
        refs.pop();
        
        if(!current->try_mark()) continue;

        current->trace_refs([&](header** ref_slot) -> void {
            if(header* ref = *ref_slot) {
                refs.push(ref);
            }
        });
    }
}

void mc_garbage_collector::visit(thread_local_stack& stack){
    auto& stack_data = stack.get_thread_stack_unlocked();
    for(thread_local_stack_entry& entry : stack_data) {
        if(entry.ref_to){
            mark_object(entry.ref_to);
        }
    }
}

void mc_garbage_collector::visit(global_root& global){
    header* gvar = global.get_global_variable_unlocked();
    if(gvar){
        mark_object(gvar);
    }
}

void mc_garbage_collector::visit(register_root& reg){
    header* reg_var = reg.get_register_variable_unlocked();
    if(reg_var){
        mark_object(reg_var);
    }
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
    auto& stack_data = stack.get_thread_stack_unlocked();
    for(thread_local_stack_entry& entry : stack_data) {
        if(entry.ref_to && entry.ref_to->forwarding_address) {
            entry.ref_to = entry.ref_to->forwarding_address;
        }
    }
}

void mc_garbage_collector::forward(global_root& global){
    header*& gvar = global.get_global_variable_unlocked();
    if(gvar && gvar->forwarding_address){
        gvar = gvar->forwarding_address;
    }
}

void mc_garbage_collector::forward(register_root& reg){
    header*& reg_var = reg.get_register_variable_unlocked();
    if(reg_var && reg_var->forwarding_address){
        reg_var = reg_var->forwarding_address;
    }
}

void mc_garbage_collector::mark(root_set_table& root_set) noexcept {
    const size_t total = root_set.get_root_count();
    if(total == 0) return;

    std::latch completion_latch(static_cast<std::ptrdiff_t>(total));

    auto& roots_table = root_set.get_roots();
    auto** buckets = roots_table.get_buckets();
    const size_t capacity = roots_table.get_capacity();

    for(size_t i = 0; i < capacity; ++i) {
        for(auto* root = buckets[i]; root; root = root->next){
            auto* root_obj = root->value;
            gc_thread_pool.enqueue(
                [&, root_obj] -> void {
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
    uint8_t* free_ptr = seg.segment_memory;
    uint8_t* scan_ptr = seg.segment_memory;
    const uint8_t* end_ptr = seg.segment_memory + cfg::heap::SEGMENT_SIZE;

    while(scan_ptr + sizeof(header) <= end_ptr) {
        header* hdr = reinterpret_cast<header*>(scan_ptr);
        const size_t object_size = sizeof(header) + static_cast<size_t>(hdr->size);

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

    std::latch completion_latch(cfg::heap::TOTAL_SEGMENTS);

    auto enqueue_segment_forward = [&](segment& segment) -> void {
        gc_thread_pool.enqueue(
            [&, seg = &segment] -> void {
                compute_forwarding_addresses_segment(*seg);
                completion_latch.count_down();
            }
        );
    };

    for(size_t i = 0; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_small_object_segment(i));
    }

    for(size_t i = 0; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_medium_object_segment(i));
    }

    for(size_t i = 0; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_large_object_segment(i));
    }

    completion_latch.wait();
}

void mc_garbage_collector::update_roots(root_set_table& root_set) noexcept {
    const size_t total = root_set.get_root_count();
    if(total == 0) return;

    std::latch completion_latch(static_cast<std::ptrdiff_t>(total));

    auto& roots_table = root_set.get_roots();
    auto** buckets = roots_table.get_buckets();
    const size_t capacity = roots_table.get_capacity();

    for(size_t i = 0; i < capacity; ++i) {
        for(auto* root = buckets[i]; root; root = root->next) {
            auto* root_obj = root->value;
            gc_thread_pool.enqueue(
                [&, root_obj] -> void {
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
    uint8_t* scan_ptr = seg.segment_memory;
    const uint8_t* end_ptr = seg.segment_memory + cfg::heap::SEGMENT_SIZE;

    while(scan_ptr + sizeof(header) <= end_ptr) {
        header* hdr = reinterpret_cast<header*>(scan_ptr);

        if(hdr->forwarding_address){
            forward_object(hdr);
        }

        scan_ptr += sizeof(header) + static_cast<size_t>(hdr->size);
    }
}

void mc_garbage_collector::update_heap_refs(heap& heap_memory) noexcept {
    if constexpr (cfg::heap::TOTAL_SEGMENTS == 0) return;

    std::latch completion_latch(cfg::heap::TOTAL_SEGMENTS);

    auto enqueue_segment_forward = [&](segment& segment) -> void {
        gc_thread_pool.enqueue(
            [&, seg = &segment] -> void {
                update_segment_refs(*seg);
                completion_latch.count_down();
            }
        );
    };

    for(size_t i = 0; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_small_object_segment(i));
    }

    for(size_t i = 0; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_medium_object_segment(i));
    }

    for(size_t i = 0; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_forward(heap_memory.get_large_object_segment(i));
    }

    completion_latch.wait();
}

void mc_garbage_collector::compact_segment(segment& seg, segment_info* seg_info) noexcept {
    uint8_t* scan_ptr = seg.segment_memory;
    const uint8_t* end_ptr = seg.segment_memory + cfg::heap::SEGMENT_SIZE;
    uint32_t used_bytes = 0;

    while(scan_ptr + sizeof(header) <= end_ptr) {
        header* hdr = reinterpret_cast<header*>(scan_ptr);
        const size_t object_size = sizeof(header) + static_cast<size_t>(hdr->size);

        if(hdr->forwarding_address) {
            header* dest = hdr->forwarding_address;
            if(dest != hdr){
                std::memmove(dest, hdr, object_size);
            }
            dest->forwarding_address = nullptr;
            dest->set_marked(false);
            used_bytes += object_size;
        }

        scan_ptr += object_size;
    }

    const uint32_t free_bytes = cfg::heap::SEGMENT_SIZE - used_bytes;
    if(static_cast<size_t>(free_bytes) > sizeof(header)){
        header* free_hdr = reinterpret_cast<header*>(seg.segment_memory + used_bytes);
        free_hdr->size = free_bytes - sizeof(header);
        free_hdr->set_free(true);
        free_hdr->next = nullptr;

        seg_info->free_list_head = free_hdr;
        seg_info->free_bytes = free_hdr->size;
    }

}

void mc_garbage_collector::compact(heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept {
    if constexpr (cfg::heap::TOTAL_SEGMENTS == 0) return;

    std::latch completion_latch(cfg::heap::TOTAL_SEGMENTS);

    auto enqueue_segment_compact = [&](segment& segment, size_t absolute_idx) -> void {
        gc_thread_pool.enqueue(
            [&, seg = &segment, absolute_idx] -> void {
                compact_segment(*seg, free_memory_table.get_segment_info(absolute_idx));
                completion_latch.count_down();
            }
        );
    };

    size_t absolute_idx = 0;
    for(size_t i = 0; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_compact(heap_memory.get_small_object_segment(i), absolute_idx++);
    }

    for(size_t i = 0; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_compact(heap_memory.get_medium_object_segment(i), absolute_idx++);
    }

    for(size_t i = 0; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_compact(heap_memory.get_large_object_segment(i), absolute_idx++);
    }

    completion_latch.wait();
}