#include "cli.hpp"

#include <charconv>
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
            else if(arg == "-o" || arg == "--output"){
                require_argument(arg, i, argc);
                handle_output_arg(options, argv[i]);
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

    void handle_mode_arg(cli_options& options, std::string_view mode_arg){
        if(mode_arg == "stress"){
            options.mode = simulation_mode::stress;
            return;
        }
        else if(mode_arg == "relaxed"){
            options.mode = simulation_mode::relaxed;
            return;
        }

        throw std::runtime_error(
            std::format("Invalid simulation mode: {}", mode_arg)
        );
        
    }

    void handle_gc_type_arg(cli_options& options, std::string_view gc_type_arg){
        if(gc_type_arg == "mc"){
            options.gc_type = garbage_collector_type::mark_compact;
            return;
        }
        else if(gc_type_arg == "ms"){
            options.gc_type = garbage_collector_type::mark_sweep;
            return;
        }

        throw std::runtime_error(
            std::format("Invalid garbage collector type: {}", gc_type_arg)
        );
    }

    void handle_iteration_arg(cli_options& options, std::string_view iteration_arg){
        int iterations{};

        auto result = std::from_chars(
            iteration_arg.data(),
            iteration_arg.data() + iteration_arg.size(),
            iterations
        );
        
        if(result.ec == std::errc() && iterations > 0){
            options.iterations = static_cast<size_t>(iterations);
            return;
        }

        throw std::runtime_error(
            std::format("Invalid mutator count: {}", iterations)
        );
    }

    void handle_mutator_arg(cli_options& options, std::string_view mutator_arg){
        int mutators{};

        auto [ptr, ec] = std::from_chars(
            mutator_arg.data(),
            mutator_arg.data() + mutator_arg.size(),
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

    void handle_output_arg(cli_options& options, std::string_view output_arg){
        if(options.output.empty()){
            options.output = output_arg;
            return;
        }
        throw std::runtime_error("Tried to set output path multiple times");
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
        std::cout << "  -o, --output               output file for simulation results\n";
        std::cout << "  -h, --help                 display this help text\n\n";
    }

}