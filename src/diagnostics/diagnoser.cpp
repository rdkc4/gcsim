#include "diagnoser.hpp"

#include <chrono>
#include <fstream>

#include "diagnostics.hpp"

diagnoser::diagnoser(cli::cli_options& options) : options{ options} {
    std::cout << "====================Simulation Environment====================\n";
    std::cout << "Number of mutators      : " << options.mutators << "\n";
    std::cout << "Number of iterations    : " << options.iterations << "\n";
    std::cout << "Simulation mode         : " << simulation_mode_name(options.mode) << "\n";
    std::cout << "Garbage collector type  : " << garbage_collector_type_name(options.gc_type) << "\n";
    std::cout << "==============================================================\n\n";
}

void diagnoser::record(diagnostics diagnostic){
    diagnostic_records.push(std::move(diagnostic));
}

void diagnoser::report(std::ostream& out) const noexcept {
    out << "======================Diagnostic Reports======================\n";
    if(diagnostic_records.empty()){
        out << "No diagnostics recorded\n";
        return;
    }

    for(const auto& diagnostic_record : diagnostic_records){
        out << diagnostic_record.report() << "\n";
    }
    out << "==============================================================\n\n";
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

    const size_t count{ diagnostic_records.get_size() };

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

    out << "===================Average Diagnostic Report==================\n";
    out << average_diagnostic_record.report();
    out << "==============================================================\n\n";
}

void diagnoser::export_report(){
    if(options.output.empty()){
        return;
    }

    std::ofstream out{ options.output };
    if(!out){
        std::cerr << std::format("Failed to open file '{}'", options.output);
        return;
    }

    out << "mutators,mode,gc_type,duration_ms,allocs,throughput_a_ms,failed_allocs,fail_rate_pct\n";
    for(const auto& diagnostic : diagnostic_records){
        out << std::format("{},{},{},{}\n", 
            options.mutators,
            simulation_mode_name(options.mode),
            garbage_collector_type_name(options.gc_type),
            diagnostic.report_csv()
        );
    }
}