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
        const auto duration_ms{ duration.count() };

        const double throughput{ 
            duration_ms > 0
                ? (static_cast<double>(allocs) * 1000.0) / duration_ms
                : 0.0
        };

        const double failure_pct{ 
            allocs > 0
                ? (static_cast<double>(failed_allocs) * 100.0) / allocs
                : 0.0
        };

        return std::format(
            "Execution time                : {} ms\n"
            "Total allocations             : {} objects\n"
            "Allocation throughput         : {:.2f} allocs/s\n"
            "Failed allocations            : {} / {} allocs\n"
            "Failed allocation percentage  : {:.2f}%\n",
            duration.count(),
            allocs,
            throughput,
            failed_allocs, allocs,
            failure_pct
        );
    }

    /**
     * @brief formats the diagnostic record as csv record.
     * @returns formatted csv record.
    */
    std::string report_csv() const noexcept {
        const auto duration_ms{ duration.count() };

        const double throughput{ 
            duration_ms > 0
                ? (static_cast<double>(allocs) * 1000.0) / duration_ms
                : 0.0
        };

        const double failure_pct{ 
            allocs > 0
                ? (static_cast<double>(failed_allocs) * 100.0) / allocs
                : 0.0
        };

        return std::format("{},{},{:.2f},{},{:.2f}",
            duration.count(),
            allocs,
            throughput,
            failed_allocs,
            failure_pct
        );
    }

};

#endif