#include <iostream>
#include <utility>

#include "src/allocators/allocators.hpp"
#include "src/diagnostics/diagnostics.hpp"
#include "src/heap-manager/heap-manager.hpp"
#include "src/common/cfg/threads-cfg.hpp"
#include "src/garbage-collector/ms-garbage-collector.hpp"

int main() {
    heap_manager heap_mng(
        std::in_place_type<ms_garbage_collector>,
        cfg::threads::GC_THREAD_COUNT
    );

    for(size_t i = 0; i < cfg::threads::ALLOC_THREAD_COUNTS_LENGTH; ++i){
        std::cout << std::format("Allocators using {} threads in stress mode: \n", cfg::threads::ALLOC_THREAD_COUNTS[i]);
        allocators allocator(heap_mng, cfg::threads::ALLOC_THREAD_COUNTS[i]);
        diagnostics diagnostic_record {
            allocator.simulate_alloc(
                cfg::threads::ALLOC_THREAD_COUNTS[i],
                simulation_mode::stress
            )
        };
        std::cout << diagnostic_record.report();
        std::cout << "\n";
    }
    
    return 0;
}
