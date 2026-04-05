#ifndef CLI_HPP
#define CLI_HPP

#include <stdexcept>
#include <format>
#include <string_view>

#include "../allocators/allocator-defs.hpp"

/**
 * @namespace cli
 * @brief module defining the cli flags for the simulator.
*/
namespace cli {
    /**
     * @struct cli_options
     * @brief struct defining the cli options for the simulator.
    */
    struct cli_options {
        /// number of concurrent mutators, defaults to 1.
        size_t mutators{1};

        /// number of simulation iterations, defaults to 1.
        size_t iterations{1};

        /// mode of the simulation, defaults to stress.
        simulation_mode mode{simulation_mode::stress};

        /// flag if help should be displayed.
        bool help{false};

        /// type of the garbage collector, defaults to mark-sweep.
        garbage_collector_type gc_type{garbage_collector_type::mark_sweep};

    };

    /**
     * @brief requires additional argument.
     * @param arg - current argument.
     * @param i - reference to the index of the argument in cli.
     * @param argc - number of arguments.
     * @throws runtime error when required argument is not provided.
    */
    inline void require_argument(std::string_view arg, int& i, int argc){
        if(++i >= argc){
            throw std::runtime_error(
                std::format("{} requires argument", arg)
            );
        }
    }

    /**
     * @brief parses arguments from the cli.
     * @param argc - number of arguments.
     * @param argv - value of arguments.
    */
    cli_options parse_args(int argc, char** argv);

    /**
     * @brief handles argument for garbage collector type.
     * @param options - reference to cli options.
     * @param gc_type_arg - garbage collector type argument.
     * @throws runtime error when gc_type_arg is invalid type.
    */
    void handle_gc_type_arg(cli_options& options, const char* gc_type_arg);

    /**
     * @brief handles argument for simulator iterations.
     * @param options - reference to cli options.
     * @param iteration_arg - argument for iteration count.
     * @throws runtime error when iteration_arg is not positive.
    */
    void handle_iteration_arg(cli_options& options, const char* iteration_arg);

    /**
     * @brief handles argument for simulation mode.
     * @param options - reference to cli options.
     * @param mode_arg - simulation mode argument.
     * @throws runtime error when mode_arg is invalid mode.
    */
    void handle_mode_arg(cli_options& options, const char* mode_arg);

    /**
     * @brief handles argument for mutators.
     * @param options - reference to cli options.
     * @param mutator_arg - argument for mutator count.
     * @throws runtime error when mutator_arg is invalid number.
    */
    void handle_mutator_arg(cli_options& options, const char* mutator_arg);

    /**
     * @brief displays the heap manager simulator usage and options.
    */
    void show_help();

};

#endif