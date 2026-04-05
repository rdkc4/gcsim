#include <exception>
#include <format>
#include <iostream>
#include <utility>

#include "src/allocators/allocator-defs.hpp"
#include "src/allocators/allocators.hpp"
#include "src/cli/cli.hpp"
#include "src/diagnostics/diagnoser.hpp"
#include "src/garbage-collector/mc-garbage-collector.hpp"
#include "src/heap-manager/heap-manager.hpp"
#include "src/common/cfg/threads-cfg.hpp"
#include "src/garbage-collector/ms-garbage-collector.hpp"

/**
 * Usage: 
 * ./gcsim [options]
 * 
 * Options:
 * -gc, --garbage-collector : type of the garbage collector: mc, ms (mc - mark-compact, ms - mark-sweep)
 * -i, --iterations         : number of simulation iterations, positive number
 * -m, --mode               : simulation mode: stress, relaxed
 * -M, --mutators           : number of concurrent mutators min 1, max 10
 * -h, --help               : display this help text
*/
int main(int argc, char** argv){
    cli::cli_options options;
    try {
        options = cli::parse_args(argc, argv);
    }
    catch(std::exception& ex){
        std::cerr << std::format(
            "Failed to run heap manager simulation, try `./gcsim --help`\n\n{}", ex.what()
        );
        return 1;
    }

    if(options.help == true){
        cli::show_help();

        if(argc == 2){
            return 0;
        }
    }

    heap_manager heap_mng{
        (options.gc_type == garbage_collector_type::mark_sweep)
            ? heap_manager(
                std::in_place_type<ms_garbage_collector>,
                cfg::threads::GC_THREAD_COUNT
            )
            : heap_manager(
                std::in_place_type<mc_garbage_collector>,
                cfg::threads::GC_THREAD_COUNT
            )
    };

    diagnoser diagnoser{ options };

    allocators allocator(heap_mng, options.mutators);
    for(size_t i{0}; i < options.iterations; ++i){
        diagnoser.record(
            allocator.simulate_alloc(
                options.mutators,
                options.mode
            )
        );
    }

    diagnoser.report();
    diagnoser.report_avg();
    
    return 0;
}
