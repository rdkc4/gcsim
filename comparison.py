import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sb
import os

# configuration
gc_types = ["mc", "ms"]
mutators_list = [1, 2, 5, 10]
iterations = 100
mode = "stress"
simulator_cmd = "./gcsim"
data_dir = "data"
plots_dir = "plots"
os.makedirs(data_dir, exist_ok=True)
os.makedirs(plots_dir, exist_ok=True)
output_pattern = os.path.join(data_dir, "results_{gc}_{mutators}.csv")

# run simulations
for gc in gc_types:
    for mutators in mutators_list:
        output_file = output_pattern.format(gc=gc, mutators=mutators)
        print(f"Running {gc} GC with {mutators} mutators -> {output_file}")
        subprocess.run([
            simulator_cmd,
            "-gc", gc,
            "-i", str(iterations),
            "-m", mode,
            "-M", str(mutators),
            "-o", output_file
        ], check=True)

# aggregate results
dfs = []
for gc in gc_types:
    for mutators in mutators_list:
        file = output_pattern.format(gc=gc, mutators=mutators)
        df = pd.read_csv(file)
        df['gc_type'] = gc
        df['mutators'] = mutators
        df['mode'] = mode
        if 'iteration' not in df.columns:
            df['iteration'] = range(1, len(df)+1)
        dfs.append(df)

all_results = pd.concat(dfs, ignore_index=True)

# plotting style 
sb.set_theme(style="whitegrid", palette="tab10")

# 1) throughput vs mutators (avg)
numeric_cols = all_results.select_dtypes(include='number').columns
numeric_cols = numeric_cols.drop(['mutators'], errors='ignore')
avg_results = all_results.groupby(['gc_type', 'mutators'])[numeric_cols].mean().reset_index()

plt.figure(figsize=(8, 5))
sb.lineplot(
    data=avg_results,
    x="mutators",
    y="throughput_a_ms",
    hue="gc_type",
    markers=True,
    dashes=False
)
plt.title("Average GC Throughput vs Mutators")
plt.xlabel("Number of Mutators")
plt.ylabel("Throughput (allocs/ms)")
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "throughput_vs_mutators.png"))
plt.show()

# 2) fail rate vs mutators (avg)
plt.figure(figsize=(8, 5))
sb.lineplot(
    data=avg_results,
    x="mutators",
    y="fail_rate_pct",
    hue="gc_type",
    markers=True,
    dashes=False
)
plt.title("Average GC Fail Rate vs Mutators")
plt.xlabel("Number of Mutators")
plt.ylabel("Failure Rate (%)")
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "fail_rate_vs_mutators.png"))
plt.show()

# 3) throughput over iterations
all_results['label'] = all_results['gc_type'] + " | " + all_results['mutators'].astype(str) + " mut"

plt.figure(figsize=(10, 6))
sb.lineplot(
    data=all_results,
    x="iteration",
    y="throughput_a_ms",
    hue="label",
    dashes=False
)
plt.title("GC Throughput per Iteration")
plt.xlabel("Iteration")
plt.ylabel("Throughput (allocs/ms)")
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "throughput_over_iterations.png"))
plt.show()

# 4) fail rate over iterations
plt.figure(figsize=(10, 6))
sb.lineplot(
    data=all_results,
    x="iteration",
    y="fail_rate_pct",
    hue="label",
    dashes=False
)
plt.title("GC Fail Rate per Iteration")
plt.xlabel("Iteration")
plt.ylabel("Failure Rate (%)")
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "fail_rate_over_iterations.png"))
plt.show()

print("All plots saved in 'plots/' folder.")