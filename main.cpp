#include <iostream>

#include "src/allocators/allocators.hpp"
#include "src/heap-manager/heap-manager.hpp"
#include "src/common/cfg/threads-cfg.hpp"

int main() {
    heap_manager heap_mng(
        cfg::threads::HM_THREAD_COUNT, 
        cfg::threads::GC_THREAD_COUNT
    );

    constexpr size_t tls_count = 5;
    constexpr size_t global_count = 5;
    constexpr size_t register_count = 5;

    for(size_t i = 0; i < cfg::threads::ALLOC_THREAD_COUNTS_LENGTH; ++i){
        std::cout << std::format("Allocators using {} threads in stress mode: \n", cfg::threads::ALLOC_THREAD_COUNTS[i]);
        allocators allocator(heap_mng, cfg::threads::ALLOC_THREAD_COUNTS[i]);
        allocator.simulate_alloc(
            tls_count, 
            global_count, 
            register_count, 
            simulation_mode::stress
        );
        std::cout << "\n";
    }
    
    return 0;
}
