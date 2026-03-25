#include "ms-garbage-collector.hpp"

#include <cassert>
#include <cstdint>
#include <latch>

#include "../common/cfg/heap-cfg.hpp"
#include "../common/indexed-stack/indexed-stack.hpp"

ms_garbage_collector::ms_garbage_collector(size_t thread_count) : garbage_collector{ thread_count } {}

void ms_garbage_collector::collect(root_set_table& root_set, heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept {
    mark(root_set);
    sweep(heap_memory, free_memory_table);
}

void ms_garbage_collector::mark_object(header* hdr) noexcept {
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

void ms_garbage_collector::visit(thread_local_stack& stack){
    auto& stack_data = stack.get_thread_stack_unlocked();
    for(thread_local_stack_entry& entry : stack_data) {
        if(entry.ref_to){
            mark_object(entry.ref_to);
        }
    }
}

void ms_garbage_collector::visit(global_root& global){
    header* gvar = global.get_global_variable_unlocked();
    if(gvar){
        mark_object(gvar);
    }
}

void ms_garbage_collector::visit(register_root& reg){
    header* reg_var = reg.get_register_variable_unlocked();
    if(reg_var){
        mark_object(reg_var);
    }
}

void ms_garbage_collector::mark(root_set_table& root_set) noexcept {
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

void ms_garbage_collector::sweep_and_coalesce_segment(segment& seg, segment_info* seg_info) noexcept {
    assert(seg_info != nullptr);

    uint8_t* current_ptr = seg.segment_memory;
    const uint8_t* end_ptr = current_ptr + cfg::heap::SEGMENT_SIZE;

    header* free_list_head = nullptr;
    header* free_list_tail = nullptr;

    uint32_t free_bytes = 0;

    while(current_ptr + sizeof(header) <= end_ptr){
        header* hdr = reinterpret_cast<header*>(current_ptr);
        if(current_ptr + sizeof(header) + static_cast<size_t>(hdr->size) > end_ptr) [[unlikely]] {
            break;
        }
        hdr->next = nullptr;

        if(hdr->is_marked()){
            hdr->set_marked(false);
            hdr->set_free(false);
        }
        else {
            hdr->set_free(true);
            
            uint8_t* next_ptr = current_ptr + sizeof(header) + static_cast<size_t>(hdr->size);
            while(next_ptr + sizeof(header) <= end_ptr){
                header* next_hdr = reinterpret_cast<header*>(next_ptr);

                if(next_ptr + sizeof(header) + static_cast<size_t>(next_hdr->size) > end_ptr) [[unlikely]] {
                    break;
                }

                if(next_hdr->is_marked()){
                    break;
                }
                next_hdr->set_free(true);
                next_hdr->next = nullptr;

                hdr->size += static_cast<uint32_t>(sizeof(header)) + next_hdr->size;
                next_ptr = current_ptr + sizeof(header) + static_cast<size_t>(hdr->size); 
            }

            if(free_list_tail) [[likely]] {
                free_list_tail->next = hdr;
            }
            else {
                free_list_head = hdr;
            }
            free_list_tail = hdr;
            free_bytes += hdr->size + static_cast<uint32_t>(sizeof(header));
        }

        current_ptr = current_ptr + sizeof(header) + static_cast<size_t>(hdr->size);
    }

    seg_info->free_list_head = free_list_head;
    seg_info->free_bytes = free_bytes;
}

void ms_garbage_collector::sweep(heap& heap_memory, segment_free_memory_table& free_memory_table) noexcept {
    if constexpr (cfg::heap::TOTAL_SEGMENTS == 0) return;
    
    std::latch completion_latch(cfg::heap::TOTAL_SEGMENTS);

    auto enqueue_segment_sweep = [&](segment& segment, size_t absolute_idx) -> void {
        gc_thread_pool.enqueue([&, seg = &segment, absolute_idx] -> void {
            sweep_and_coalesce_segment(*seg, free_memory_table.get_segment_info(absolute_idx));
            completion_latch.count_down();
        });
    };

    size_t absolute_idx = 0;
    for(size_t i = 0; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_sweep(heap_memory.get_small_object_segment(i), absolute_idx++);
    }

    for(size_t i = 0; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_sweep(heap_memory.get_medium_object_segment(i), absolute_idx++);
    }

    for(size_t i = 0; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        enqueue_segment_sweep(heap_memory.get_large_object_segment(i), absolute_idx++);
    }

    completion_latch.wait();
}
