#ifndef DIAGNOSER_HPP
#define DIAGNOSER_HPP

#include <ostream>
#include <iostream>
#include <utility>
#include <string_view>

#include "../common/stack/indexed-stack.hpp"
#include "../cli/cli.hpp"
#include "diagnostics.hpp"

/**
 * @class diagnoser
 * @brief handler for diagnostics.
*/
class diagnoser {
private:
    /// simulation options.
    cli::cli_options& options;

    /// stack of diagnostic records.
    indexed_stack<diagnostics> diagnostic_records;

    /**
     * @brief getter for the name of the mode of simulation.
     * @param mode - simulation mode.
     * @returns name of the mode.
    */
    constexpr std::string_view simulation_mode_name(simulation_mode mode) noexcept {
        switch(mode){
            case simulation_mode::stress: return "stress";
            case simulation_mode::relaxed: return "relaxed";
        }
        std::unreachable();
    }

    /**
     * @brief getter for the type of the garbage collector.
     * @param gc_type - type of the garbage collector.
     * @returns name of the garbage collector type.
    */
    constexpr std::string_view garbage_collector_type_name(garbage_collector_type gc_type) noexcept {
        switch(gc_type){
            case garbage_collector_type::mark_sweep: return "mark-sweep";
            case garbage_collector_type::mark_compact: return "mark-compact";
        }

        std::unreachable();
    }

public:
    /**
     * @brief creates the instance of the diagnoser.
     * @param options - simulation options.
    */
    diagnoser(cli::cli_options& options);

    /**
     * @brief deletes the instance of the diagnoser.
    */
    ~diagnoser() = default;

    /**
     * @brief creates new diagnostic record.
     * @param diagnostic - new diagnostic record.
    */
    void record(diagnostics diagnostic);

    /**
     * @brief reports all diagnostic records.
     * @param out - output stream.
    */
    void report(std::ostream& out = std::cout) const noexcept;

    /**
     * @brief reports average diagnostic data.
     * @param out - output stream.
    */
    void report_avg(std::ostream& out = std::cout) const noexcept;

    /**
     * @brief exports simulation data to csv file.
    */
    void export_report();

};

#endif