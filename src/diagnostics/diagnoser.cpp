#include "diagnoser.hpp"
#include "diagnostics.hpp"
#include <chrono>

void diagnoser::record(diagnostics diagnostic){
    diagnostic_records.push(std::move(diagnostic));
}

void diagnoser::report(std::ostream& out) const noexcept {
    if(diagnostic_records.empty()){
        out << "No diagnostics recorded\n";
        return;
    }

    for(const auto& diagnostic_record : diagnostic_records){
        out << diagnostic_record.report() << "\n";
    }
}

void diagnoser::report_avg(std::ostream& out) const noexcept {
    if(diagnostic_records.empty()){
        out << "No diagnostics recorded\n";
        return;
    }

    __int128 total_allocs{0};
    __int128 total_failed_allocs{0};
    __int128 total_duration{0};

    for(const auto& diagnostic_record : diagnostic_records){
        total_allocs += diagnostic_record.allocs;
        total_failed_allocs += diagnostic_record.failed_allocs;
        total_duration += diagnostic_record.duration.count();
    }

    const size_t count = diagnostic_records.get_size();

    size_t avg_allocs = total_allocs / count;
    size_t avg_failed_allocs = total_failed_allocs / count;
    std::chrono::milliseconds avg_duration = std::chrono::milliseconds(
        static_cast<long long>(total_duration / count)
    );

    diagnostics average_diagnostic_record {
        .allocs = avg_allocs,
        .failed_allocs = avg_failed_allocs,
        .duration = avg_duration
    };

    out << average_diagnostic_record.report();
}