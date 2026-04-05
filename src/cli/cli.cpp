#include "cli.hpp"

#include <charconv>
#include <cstring>
#include <system_error>
#include <iostream>

namespace cli {
    cli_options parse_args(int argc, char **argv){
        cli_options options;

        for(int i{1}; i < argc; ++i){
            std::string_view arg{ argv[i] };

            if(arg == "-gc" || arg == "--garbage-collector"){
                require_argument(arg, i, argc);
                handle_gc_type_arg(options, argv[i]);
            }
            else if(arg == "-i" || arg == "--iterations"){
                require_argument(arg, i, argc);
                handle_iteration_arg(options, argv[i]);
            }
            else if(arg == "-m" || arg == "--mode"){
                require_argument(arg, i, argc);
                handle_mode_arg(options, argv[i]);
            }
            else if(arg == "-M" || arg == "--mutators"){
                require_argument(arg, i, argc);
                handle_mutator_arg(options, argv[i]);
            }
            else if(arg == "-h" || arg == "--help"){
                options.help = true;
            }
            else {
                throw std::runtime_error(
                    std::format("Invalid argument: {}", arg)
                );
            }

        }

        return options;
    }

    void handle_mode_arg(cli_options& options, const char* mode_arg){
        if(std::strcmp(mode_arg, "stress") == 0){
            options.mode = simulation_mode::stress;
            return;
        }
        else if(std::strcmp(mode_arg, "relaxed") == 0){
            options.mode = simulation_mode::relaxed;
            return;
        }

        throw std::runtime_error(
            std::format("Invalid simulation mode: {}", mode_arg)
        );
        
    }

    void handle_gc_type_arg(cli_options& options, const char* gc_type_arg){
        if(std::strcmp(gc_type_arg, "mc") == 0){
            options.gc_type = garbage_collector_type::mark_compact;
            return;
        }
        else if(std::strcmp(gc_type_arg, "ms") == 0){
            options.gc_type = garbage_collector_type::mark_sweep;
            return;
        }

        throw std::runtime_error(
            std::format("Invalid garbage collector type: {}", gc_type_arg)
        );
    }

    void handle_iteration_arg(cli_options& options, const char* iteration_arg){
        int iterations{};

        auto [ptr, ec] = std::from_chars(
            iteration_arg,
            iteration_arg + std::strlen(iteration_arg),
            iterations
        );
        
        if(ec == std::errc() && iterations > 0){
            options.iterations = static_cast<size_t>(iterations);
            return;
        }

        throw std::runtime_error(
            std::format("Invalid mutator count: {}", iterations)
        );
    }

    void handle_mutator_arg(cli_options& options, const char* mutator_arg){
        int mutators{};

        auto [ptr, ec] = std::from_chars(
            mutator_arg,
            mutator_arg + std::strlen(mutator_arg),
            mutators
        );
        
        if(ec == std::errc() && mutators > 0 && mutators <= 10){
            options.mutators = static_cast<size_t>(mutators);
            return;
        }

        throw std::runtime_error(
            std::format("Invalid mutator count: {}", mutator_arg)
        );
    }

    void show_help(){
        std::cout << "===============================Heap Manager Simulator===============================\n";
        std::cout << "========================================HELP========================================\n\n";
        std::cout << "Usage:\n";
        std::cout << "  ./gcsim [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -gc, --garbage-collector   type of the garbage collector: mc, ms\n";
        std::cout << "                             mc - mark-compact, ms - mark-sweep\n";
        std::cout << "  -i, --iterations           number of simulation iterations, positive number\n";
        std::cout << "  -m, --mode                 simulation mode: stress, relaxed\n";
        std::cout << "  -M, --mutators             number of concurrent mutators min 1, max 10\n";
        std::cout << "  -h, --help                 display this help text\n\n";
    }

}