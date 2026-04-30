#!/usr/bin/env python3
"""!
@brief runs GC simulations and generates performance plots.

@details
Script:
- Executes the GC simulator with different configurations
- Aggregates CSV results
- Produces plots for throughput and failure rate

Configurations:
- GC types: mc, ms
- Mutator counts: 1, 2, 5, 10
- Modes: stress, relaxed

Outputs:
- Raw CSV data in `data/`
- Plots in `plots/`
"""

import subprocess
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import seaborn as sb
import os

def run_simulations(
    gc_types: list[str], 
    mutators_list: list[int], 
    modes: list[str], 
    iterations: int, 
    simulator_cmd: str, 
    output_pattern: str
) -> None:
    """
    @brief run GC simulations for all configurations.

    @param gc_types - list of GC types (["mc", "ms"])
    @param mutators_list - list of mutator counts
    @param modes - list of execution modes (["stress", "relaxed"])
    @param iterations - number of iterations per run
    @param simulator_cmd - path to simulator executable
    @param output_pattern - output filename pattern

    @throws subprocess.CalledProcessError if a simulation fails
    """
    for mode in modes:
        for gc in gc_types:
            for mutators in mutators_list:
                output_file = output_pattern.format(
                    gc=gc, mode=mode, mutators=mutators
                )

                print(f"Running {gc} GC with {mutators} mutators in {mode} mode -> {output_file}")

                subprocess.run([
                    simulator_cmd,
                    "-gc", gc,
                    "-i", str(iterations),
                    "-m", mode,
                    "-M", str(mutators),
                    "-o", output_file
                ], check=True)

def load_results(
    gc_types: list[str], 
    mutators_list: list[int], 
    modes: list[str], 
    output_pattern: str
) -> pd.DataFrame:
    """
    @brief load and aggregate simulation results.

    @param gc_types - list of GC types
    @param mutators_list - list of mutator counts
    @param modes - list of execution modes
    @param output_pattern - output filename pattern

    @return pandas.DataFrame Combined dataset of all results
    """
    dfs: list[pd.DataFrame] = []

    for mode in modes:
        for gc in gc_types:
            for mutators in mutators_list:
                file = output_pattern.format(
                    gc=gc, mode=mode, mutators=mutators
                )

                df = pd.read_csv(file)
                df['gc_type'] = gc
                df['mutators'] = mutators
                df['mode'] = mode
                df['iteration'] = range(1, len(df) + 1)

                dfs.append(df)

    return pd.concat(dfs, ignore_index=True)

def plot_mode_results(
    mode_results: pd.DataFrame, 
    mode: str, 
    plots_dir: str
) -> None:
    """
    @brief generates all plots for a single mode.

    @param mode_results - DataFrame filtered by mode
    @param mode - current mode name
    @param plots_dir - directory to save plots
    """

    # Compute averages
    numeric_cols = mode_results.select_dtypes(include='number').columns
    numeric_cols = numeric_cols.drop(['mutators', 'iteration'], errors='ignore')

    avg_results = (
        mode_results
        .groupby(['gc_type', 'mutators'])[numeric_cols]
        .mean()
        .reset_index()
    )

    # Throughput vs mutators
    plt.figure(figsize=(8, 5))
    sb.lineplot(
        data=avg_results,
        x="mutators",
        y="throughput_a_ms",
        hue="gc_type",
        markers=True,
        dashes=False
    )
    plt.title(f"Average GC Throughput vs Mutators ({mode})")
    plt.xlabel("Number of Mutators")
    plt.ylabel("Throughput (allocs/s)")
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, f"throughput_vs_mutators_{mode}.png"))
    plt.close()

    # Fail rate vs mutators
    plt.figure(figsize=(8, 5))
    sb.lineplot(
        data=avg_results,
        x="mutators",
        y="fail_rate_pct",
        hue="gc_type",
        markers=True,
        dashes=False
    )
    plt.title(f"Average GC Fail Rate vs Mutators ({mode})")
    plt.xlabel("Number of Mutators")
    plt.ylabel("Failure Rate (%)")
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, f"fail_rate_vs_mutators_{mode}.png"))
    plt.close()

    # Label for iteration plots
    mode_results = mode_results.copy()
    mode_results['label'] = (
        mode_results['gc_type'] + " | " +
        mode_results['mutators'].astype(str) + " mut"
    )

    # Throughput over iterations
    plt.figure(figsize=(10, 6))
    sb.lineplot(
        data=mode_results,
        x="iteration",
        y="throughput_a_ms",
        hue="label",
        dashes=False
    )
    plt.title(f"GC Throughput per Iteration ({mode})")
    plt.xlabel("Iteration")
    plt.ylabel("Throughput (allocs/s)")
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, f"throughput_over_iterations_{mode}.png"))
    plt.close()

    # Fail rate over iterations
    plt.figure(figsize=(10, 6))
    sb.lineplot(
        data=mode_results,
        x="iteration",
        y="fail_rate_pct",
        hue="label",
        dashes=False
    )
    plt.title(f"GC Fail Rate per Iteration ({mode})")
    plt.xlabel("Iteration")
    plt.ylabel("Failure Rate (%)")
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, f"fail_rate_over_iterations_{mode}.png"))
    plt.close()


def plot_results(
    all_results: pd.DataFrame, 
    modes: list[str], 
    plots_dir: str
) -> None:
    """
    @brief generates plots for all modes.

    @param all_results - combined DataFrame with all experiment data
    @param modes - list of modes
    @param plots_dir - output directory for plots
    """
    sb.set_theme(style="whitegrid", palette="tab10")

    for mode in modes:
        mode_results = all_results[all_results['mode'] == mode].copy()
        plot_mode_results(mode_results, mode, plots_dir)

def main() -> None:
    """
    @brief entry point for running simulations and plotting results.
    """

    # Configuration
    gc_types = ["mc", "ms"]
    mutators_list = [1, 2, 5, 10]
    iterations = 100
    modes = ["stress", "relaxed"]

    simulator_cmd = "./gcsim"
    data_dir = "data"
    plots_dir = "plots"

    os.makedirs(data_dir, exist_ok=True)
    os.makedirs(plots_dir, exist_ok=True)

    output_pattern = os.path.join(
        data_dir,
        "results_{gc}_{mode}_{mutators}.csv"
    )

    # Execute workflow
    run_simulations(
        gc_types,
        mutators_list,
        modes,
        iterations,
        simulator_cmd,
        output_pattern
    )

    all_results = load_results(
        gc_types,
        mutators_list,
        modes,
        output_pattern
    )

    plot_results(all_results, modes, plots_dir)

    print("All plots saved in 'plots/' folder.")


if __name__ == "__main__":
    main()
