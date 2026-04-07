#include "heap-manager.hpp"

#include "../common/cfg/heap-cfg.hpp"
#include "../common/cfg/heap-manager-cfg.hpp"

void heap_manager::init_free_memory(){
    for(size_t i{0}; i < cfg::heap::SMALL_OBJECT_SEGMENTS; ++i) {
        segment& segment{ heap_memory.get_small_object_segment(i) };
        header* initial_header{ reinterpret_cast<header*>(segment.segment_memory) };
        free_memory_table.update_segment(
            i, 
            initial_header, 
            segment.free_memory
        );
    }

    for(size_t i{0}; i < cfg::heap::MEDIUM_OBJECT_SEGMENTS; ++i) {
        segment& segment{ heap_memory.get_medium_object_segment(i) };
        header* initial_header{ reinterpret_cast<header*>(segment.segment_memory) };
        free_memory_table.update_segment(
            cfg::heap::SMALL_OBJECT_SEGMENTS + i, 
            initial_header, 
            segment.free_memory
        );
    }

    for(size_t i{0}; i < cfg::heap::LARGE_OBJECT_SEGMENTS; ++i) {
        segment& segment{ heap_memory.get_large_object_segment(i) };
        header* initial_header{ reinterpret_cast<header*>(segment.segment_memory) };
        free_memory_table.update_segment(
            cfg::heap::SMALL_OBJECT_SEGMENTS + cfg::heap::MEDIUM_OBJECT_SEGMENTS + i, 
            initial_header, 
            segment.free_memory
        );
    }
}

uint32_t heap_manager::calculate_free_memory() const noexcept {
    uint32_t total{0};
    for(size_t i{0}; i < cfg::heap::TOTAL_SEGMENTS; ++i){
        const segment_info* seg_info{ free_memory_table.get_segment_info(i) };
        if(!seg_info) continue;

        total += seg_info->free_bytes.load(std::memory_order_relaxed);
    }

    return total;
}

header* heap_manager::allocate(uint32_t bytes){
    safepoint_poll();

    if(bytes == 0) return nullptr;
    bytes = (bytes + cfg::heap::SEGMENT_ALIGNMENT - 1) & 
        ~(cfg::heap::SEGMENT_ALIGNMENT - 1);

    int segment_index{ find_suitable_segment(bytes) };
    if(segment_index >= 0){
        std::lock_guard<std::mutex> seg_lock(segment_locks[segment_index]);
        if(header* obj{ allocate_from_segment(static_cast<size_t>(segment_index), bytes) })
            return obj;
    }
    
    if(should_run_gc()){
        bool expected{false};
        if(gc_in_progress.compare_exchange_strong(expected, true, std::memory_order_acq_rel)){
            collect_garbage(true); //< gc is being called by mutator.
        }
        else {
            safepoint_poll();
        }

        segment_index = find_suitable_segment(bytes);
        if(segment_index >= 0){
            std::lock_guard<std::mutex> seg_lock(segment_locks[segment_index]);
            return allocate_from_segment(static_cast<size_t>(segment_index), bytes);
        }
    }

    return nullptr;
}

void heap_manager::add_root(uint64_t id, root_set_base* base){
    std::lock_guard<std::mutex> root_set_lock(root_set_mutex);
    root_set.add_root(id, base);
}

void heap_manager::remove_root(uint64_t id){
    std::lock_guard<std::mutex> root_set_lock(root_set_mutex);
    root_set.remove_root(id);
}

void heap_manager::clear_roots() noexcept {
    std::lock_guard<std::mutex> root_set_lock(root_set_mutex);
    root_set.clear();
}

void heap_manager::collect_garbage(bool called_by_mutator){
    stw_requested.store(true, std::memory_order_release);

    size_t expected_parked{ mutator_count.load(std::memory_order_acquire) };
    if(called_by_mutator && expected_parked > 0){
        --expected_parked;
    }

    {
        std::unique_lock<std::mutex> lock(stw_mutex);
        stw_cv.wait(lock, [this, expected_parked] -> bool {
            return mutators_at_safepoint >= expected_parked;
        });
    }

    {
        std::lock_guard<std::mutex> root_set_lock(root_set_mutex);
        gc->collect(root_set, heap_memory, free_memory_table);
    }

    last_gc_time_ms.store(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count(),std::memory_order_release
    );
    
    stw_requested.store(false, std::memory_order_release);
    {
        std::unique_lock<std::mutex> lock(stw_mutex);
        stw_cv.notify_all();
    }

    gc_in_progress.store(false, std::memory_order_release);
}

bool heap_manager::should_run_gc() const noexcept {
    auto now_ms{ 
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    };
    auto last_ms{ last_gc_time_ms.load(std::memory_order_acquire) };

    /// time-based gc trigger.
    if((now_ms - last_ms) >= cfg::heap_manager::MIN_GC_INTERVAL.count()){
        return true;
    }

    /// free memory-based gc trigger.
    if(calculate_free_memory() < cfg::heap_manager::GC_LOW_MEMORY_THRESHOLD){
        return true;
    }

    return false;
}

void heap_manager::periodic_gc_loop(std::stop_token stop_token){
    std::mutex periodic_gc_mutex;
    std::condition_variable gc_cv;

    std::stop_callback stop_cb(stop_token, [&gc_cv] -> void { gc_cv.notify_one(); });

    while(!stop_token.stop_requested()){
        std::unique_lock<std::mutex> gc_lock(periodic_gc_mutex);
        gc_cv.wait_for(gc_lock, cfg::heap_manager::PERIODIC_GC_INTERVAL);

        if(stop_token.stop_requested()) break;

        if(!should_run_gc()) continue;

        bool expected{false};
        if(gc_in_progress.compare_exchange_strong(expected, true, std::memory_order_acq_rel)){
            collect_garbage();
        }
    }
}

int heap_manager::find_suitable_segment(uint32_t bytes) noexcept {
    size_t start_idx{}, end_idx{};
    std::atomic<size_t>* last_segment_idx;
    int fallback_segment_idx{-1};
    uint32_t fallback_segment_size{0};

    if(bytes <= cfg::heap_manager::SMALL_OBJECT_THRESHOLD){
        start_idx = 0;
        end_idx = cfg::heap::SMALL_OBJECT_SEGMENTS;
        last_segment_idx = &last_small_segment;
    }
    else if(bytes <= cfg::heap_manager::MEDIUM_OBJECT_THRESHOLD){
        start_idx = cfg::heap::SMALL_OBJECT_SEGMENTS;
        end_idx = cfg::heap::SMALL_OBJECT_SEGMENTS + cfg::heap::MEDIUM_OBJECT_SEGMENTS;
        last_segment_idx = &last_medium_segment;
    }
    else {
        start_idx = cfg::heap::SMALL_OBJECT_SEGMENTS + cfg::heap::MEDIUM_OBJECT_SEGMENTS;
        end_idx = cfg::heap::SMALL_OBJECT_SEGMENTS + cfg::heap::MEDIUM_OBJECT_SEGMENTS + cfg::heap::LARGE_OBJECT_SEGMENTS;
        last_segment_idx = &last_large_segment;
    }

    const size_t segment_count{ end_idx - start_idx };
    size_t last_used{ last_segment_idx->load(std::memory_order_acquire) }; 
    size_t start_offset{ 
        (last_used >= start_idx && last_used < end_idx) 
            ? (last_used - start_idx) 
            : 0 
    };

    for(size_t offset{0}; offset < segment_count; ++offset){
        size_t relative_idx{ (start_offset + offset + 1) % segment_count };
        size_t idx{ start_idx + relative_idx };

        const segment_info* seg_info{ free_memory_table.get_segment_info(idx) };
        if(!seg_info) continue;

        const uint32_t free_bytes{ seg_info->free_bytes.load(std::memory_order_acquire) };
        if(free_bytes < bytes + sizeof(header)) continue;
        
        if(fallback_segment_idx == -1 || fallback_segment_size < free_bytes){
            fallback_segment_idx = idx;
            fallback_segment_size = free_bytes;
        }

        std::unique_lock<std::mutex> segment_lock(segment_locks[idx], std::try_to_lock);
        if(!segment_lock.owns_lock()) continue;

        last_segment_idx->store(idx, std::memory_order_release);
        return static_cast<int>(idx);
    }

    if(fallback_segment_idx != -1){
        last_segment_idx->store(static_cast<size_t>(fallback_segment_idx), std::memory_order_release);
    }

    return fallback_segment_idx;
}

header* heap_manager::allocate_from_segment(size_t segment_index, uint32_t bytes){
    segment_info* seg_info{ free_memory_table.get_segment_info(segment_index) };
    if(!seg_info || !seg_info->free_list_head){
        return nullptr;
    }

    header* current{ seg_info->free_list_head };
    header* prev{nullptr};

    while(current){
        if(current->size >= bytes){
            break;
        }
        prev = current;
        current = current->next;
    }

    if(!current){
        return nullptr;
    }

    uint32_t remaining{ current->size - bytes };
    if(remaining >= static_cast<uint32_t>(sizeof(header)) + cfg::heap::SEGMENT_ALIGNMENT){
        auto* new_header{ 
            reinterpret_cast<header*>(
                reinterpret_cast<uint8_t*>(current) + sizeof(header) + static_cast<size_t>(bytes)
            )
        };
        
        new_header->size = remaining - static_cast<uint32_t>(sizeof(header));
        new_header->next = current->next;
        new_header->set_free(true);
        new_header->set_marked(false);

        current->size = bytes;
        current->next = new_header;
    }

    current->set_free(false);
    current->set_marked(false);

    if(prev){
        prev->next = current->next;
    }
    else{
        seg_info->free_list_head = current->next;
    }
    current->next = nullptr;

    seg_info->free_bytes.fetch_sub(current->size + static_cast<uint32_t>(sizeof(header)));
    return current;
}

void heap_manager::register_mutator() noexcept {
    safepoint_poll();
    mutator_count.fetch_add(1, std::memory_order_relaxed);
}

void heap_manager::unregister_mutator() noexcept {
    safepoint_poll();
    mutator_count.fetch_sub(1, std::memory_order_acq_rel);
}

void heap_manager::safepoint_poll(){
    if(!stw_requested.load(std::memory_order_acquire)) return;
    std::unique_lock<std::mutex> lock(stw_mutex);

    ++mutators_at_safepoint;
    stw_cv.notify_all();

    stw_cv.wait(lock, [this] { 
        return !stw_requested.load(std::memory_order_acquire); 
    });

    --mutators_at_safepoint;
}