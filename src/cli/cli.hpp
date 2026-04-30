#ifndef CLI_HPP
#define CLI_HPP

#include <string_view>
#include <string>
#include <expected>

#include "../allocators/allocator-defs.hpp"

/**
 * @namespace cli
 * @brief module for cli option handling.
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

        /// output file for simulation results.
        std::string output{""};

    };

    /// alias for handler types.
    using handler_fn = std::expected<void, std::string>(
        cli_options&,
        std::string_view
    );

    /**
     * @struct opt_descriptor
     * @brief descriptor of the option argument.
    */
    struct opt_descriptor {
        /// name of the option argument.
        std::string_view name;

        /// pointer to the option argument's handler.
        handler_fn* handler;

        /// flag if option argument requires additional value.
        bool expects_value;
    };

    /**
     * @brief parses arguments from the cli.
     * @param argc - number of arguments.
     * @param argv - pointer to arguments.
    */
    std::expected<cli_options, std::string> parse_args(int argc, const char* const* argv);

    /**
     * @brief processes the option argument.
     * @param options - reference to cli options.
     * @param desc - pointer to a opt_descriptor of the option argument.
     * @param i - reference to index of the current argument.
     * @param argc - total number of arguments.
     * @param argv - pointer to arguments.
     * @returns void if option is valid, error string if it fails to process option.
    */
    std::expected<void, std::string> 
    process_opt(cli_options& options, const opt_descriptor* desc, int& i, int argc, const char* const* argv);

    /**
     * @brief handles argument for garbage collector type.
     * @param options - reference to cli options.
     * @param gc_type_arg - garbage collector type argument.
     * @returns error description if it fails to handle gc type.
    */
    std::expected<void, std::string> 
    handle_gc_type_arg(cli_options& options, std::string_view gc_type_arg);

    /**
     * @brief handles argument for simulator iterations.
     * @param options - reference to cli options.
     * @param iteration_arg - argument for iteration count.
     * @returns error description if it fails to handle iteration count.
    */
    std::expected<void, std::string> 
    handle_iteration_arg(cli_options& options, std::string_view iteration_arg);

    /**
     * @brief handles argument for simulation mode.
     * @param options - reference to cli options.
     * @param mode_arg - simulation mode argument.
     * @returns error description if it fails to handle simulation mode.
    */
    std::expected<void, std::string> 
    handle_mode_arg(cli_options& options, std::string_view mode_arg);

    /**
     * @brief handles argument for mutators.
     * @param options - reference to cli options.
     * @param mutator_arg - argument for mutator count.
     * @returns error description if it fails to handle mutator count.
    */
    std::expected<void, std::string> 
    handle_mutator_arg(cli_options& options, std::string_view mutator_arg);

    /**
     * @brief handles argument for output.
     * @param options - reference to cli options.
     * @param output_arg - argument for output file.
     * @returns error description if it fails to handle output file.
    */
    std::expected<void, std::string> 
    handle_output_arg(cli_options& options, std::string_view output_arg);

    /**
     * @brief handles argument for help.
     * @param options - reference to cli options.
     * @param help_arg - argument for help.
     * @note returns std::expected<void, std::string> only for compatibility with other handlers.
    */
    std::expected<void, std::string> 
    handle_help_arg(cli_options& options, [[maybe_unused]]std::string_view help_arg);

    /**
     * @brief displays the heap manager simulator usage and options.
    */
    void show_help();

    /// descriptors of cli option args.
    constexpr opt_descriptor opt_descriptors[]{
        {.name = "-gc",                 .handler = &handle_gc_type_arg,   .expects_value = true},
        {.name = "--garbage-collector", .handler = &handle_gc_type_arg,   .expects_value = true},
        {.name = "-i",                  .handler = &handle_iteration_arg, .expects_value = true},
        {.name = "--iterations",        .handler = &handle_iteration_arg, .expects_value = true},
        {.name = "-m",                  .handler = &handle_mode_arg,      .expects_value = true},
        {.name = "--mode",              .handler = &handle_mode_arg,      .expects_value = true},
        {.name = "-M",                  .handler = &handle_mutator_arg,   .expects_value = true},
        {.name = "--mutators",          .handler = &handle_mutator_arg,   .expects_value = true},
        {.name = "-o",                  .handler = &handle_output_arg,    .expects_value = true},
        {.name = "--output",            .handler = &handle_output_arg,    .expects_value = true},
        {.name = "-h",                  .handler = &handle_help_arg,      .expects_value = false},
        {.name = "--help",              .handler = &handle_help_arg,      .expects_value = false}
    };

    /**
     * @brief gets the option descriptor of corresponding arg option.
     * @param arg - argument from cli.
     * @returns pointer to opt_descriptor, nullptr when arg is unknown.
    */
    constexpr const opt_descriptor* to_opt_desc(std::string_view arg){
        for(const auto& desc : opt_descriptors){
            if(desc.name == arg){
                return &desc;
            }
        }
        return nullptr;
    }

};

#endif