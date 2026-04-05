#ifndef DIAGNOSTICS_HPP
#define DIAGNOSTICS_HPP

#include <cstddef>
#include <chrono>
#include <format>
#include <string>

/**
 * @struct diagnostics
 * @brief structure for the simulation diagnostics.
*/
struct diagnostics {
    /// total number of allocations.
    size_t allocs{};

    /// total number of failed allocations.
    size_t failed_allocs{};

    /// duration of the execution.
    std::chrono::milliseconds duration{};

    /**
     * @brief formats the diagnostic record.
     * @returns formatted diagnostic record.
    */
    std::string report() const noexcept {
        return std::format(
            "Execution time                : {} ms\n"
            "Total allocations             : {} objects\n"
            "Allocation throughput         : {:.2f} allocs/s\n"
            "Failed allocations            : {} / {} allocs\n"
            "Failed allocation percentage  : {:.2f}%\n",
            duration.count(),
            allocs,
            static_cast<double>(allocs) / duration.count() * 1000,
            failed_allocs, allocs,
            static_cast<double>(failed_allocs) / static_cast<double>(allocs) * 100
        );
    }

};

#endif