#include "cli.hpp"

#include <charconv>
#include <system_error>
#include <iostream>
#include <format>

namespace cli {
    std::expected<cli_options, std::string> parse_args(int argc, const char* const* argv){
        cli_options options;

        for(int i{1}; i < argc; ++i){
            std::string_view arg{ argv[i] };
            auto opt_desc{ to_opt_desc(arg) };

            if(!opt_desc){
                return std::unexpected(
                    std::format("Invalid argument: {}", arg)
                );
            }

            auto res{ process_opt(options, opt_desc, i, argc, argv) };
            if(!res){
                return std::unexpected(res.error());
            }
        }

        return options;
    }

    std::expected<void, std::string> process_opt(
        cli_options& options, 
        const opt_descriptor* desc,
        int& i, 
        int argc, 
        const char* const* argv
    ){
        std::string_view value{};

        if(desc->expects_value){
            if(i + 1 >= argc){
                return std::unexpected(
                    std::format("{} requires argument", desc->name)
                );
            }

            value = argv[++i];
        }

        return desc->handler(options, value);
    }

    std::expected<void, std::string> 
    handle_mode_arg(cli_options& options, std::string_view mode_arg){
        if(mode_arg == "stress"){
            options.mode = simulation_mode::stress;
            return {};
        }
        else if(mode_arg == "relaxed"){
            options.mode = simulation_mode::relaxed;
            return {};
        }

        return std::unexpected(
            std::format("Invalid simulation mode: {}", mode_arg)
        );
    }

    std::expected<void, std::string> 
    handle_gc_type_arg(cli_options& options, std::string_view gc_type_arg){
        if(gc_type_arg == "mc"){
            options.gc_type = garbage_collector_type::mark_compact;
            return {};
        }
        else if(gc_type_arg == "ms"){
            options.gc_type = garbage_collector_type::mark_sweep;
            return {};
        }

        return std::unexpected(
            std::format("Invalid garbage collector type: {}", gc_type_arg)
        );
    }

    std::expected<void, std::string> 
    handle_iteration_arg(cli_options& options, std::string_view iteration_arg){
        int iterations{};

        auto [ptr, ec]{ 
            std::from_chars(
                iteration_arg.data(),
                iteration_arg.data() + iteration_arg.size(),
                iterations
            )
        };
        
        if(ec == std::errc() && iterations > 0 && ptr == iteration_arg.data() + iteration_arg.size()){
            options.iterations = static_cast<size_t>(iterations);
            return {};
        }

        return std::unexpected(
            std::format("Invalid iteration count: {}", iterations)
        );
    }

    std::expected<void, std::string> 
    handle_mutator_arg(cli_options& options, std::string_view mutator_arg){
        int mutators{};

        auto [ptr, ec]{ 
            std::from_chars(
                mutator_arg.data(),
                mutator_arg.data() + mutator_arg.size(),
                mutators
            )
        };
        
        if(ec == std::errc() && mutators > 0 && mutators <= 10 && ptr == mutator_arg.data() + mutator_arg.size()){
            options.mutators = static_cast<size_t>(mutators);
            return {};
        }

        return std::unexpected(
            std::format("Invalid mutator count: {}", mutator_arg)
        );
    }

    std::expected<void, std::string> 
    handle_output_arg(cli_options& options, std::string_view output_arg){
        if(options.output.empty()){
            options.output = output_arg;
            return {};
        }
        return std::unexpected("Tried to set output path multiple times");
    }

    std::expected<void, std::string> 
    handle_help_arg(cli_options& options, [[maybe_unused]]std::string_view help_arg){
        options.help = true;
        return {};
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