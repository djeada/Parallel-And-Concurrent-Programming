import matplotlib.pyplot as plt
import numpy as np


def compute_total_time(num_tasks, computation_time_per_task, communication_overhead):
    """
    Compute total execution time for parallel tasks.

    Parameters:
    - num_tasks: Number of tasks
    - computation_time_per_task: Time to compute each task
    - communication_overhead: Overhead per task for communication/synchronization

    Returns:
    - Total execution time
    """
    return num_tasks * (computation_time_per_task + communication_overhead)


def compute_granularity(computation_time, communication_time):
    """
    Compute granularity as the ratio of computation to communication.

    Parameters:
    - computation_time: Total computation time
    - communication_time: Total communication time

    Returns:
    - Granularity ratio
    """
    if communication_time == 0:
        return float("inf")
    return computation_time / communication_time


# Simulation parameters
total_work = 1000  # Total units of work
num_processors = 8

# Different granularity scenarios
task_counts = np.array([10, 20, 50, 100, 200, 500, 1000])  # Number of tasks
base_comm_overhead = 5  # Base communication overhead per task

# Calculate metrics for each granularity level
computation_per_task = total_work / task_counts
total_computation = np.full_like(task_counts, total_work, dtype=float)
total_communication = task_counts * base_comm_overhead
granularities = computation_per_task / base_comm_overhead
total_times = total_computation + total_communication
efficiency = total_computation / total_times

# Create figure with multiple subplots
fig, axes = plt.subplots(2, 2, figsize=(14, 10))

# Plot 1: Granularity vs Number of Tasks
ax1 = axes[0, 0]
ax1.plot(task_counts, granularities, "b-o", linewidth=2, markersize=8)
ax1.axhline(y=1, color="r", linestyle="--", label="Granularity = 1 (balanced)")
ax1.fill_between(task_counts, 0, 1, alpha=0.2, color="red", label="Fine-grained region")
ax1.fill_between(
    task_counts, 1, max(granularities), alpha=0.2, color="green", label="Coarse-grained"
)
ax1.set_xlabel("Number of Tasks")
ax1.set_ylabel("Granularity (Computation/Communication)")
ax1.set_title("Granularity vs Task Count")
ax1.set_xscale("log")
ax1.set_yscale("log")
ax1.legend()
ax1.grid(True, which="both", ls="--", alpha=0.7)

# Plot 2: Time Breakdown
ax2 = axes[0, 1]
width = 0.35
x = np.arange(len(task_counts))
bars1 = ax2.bar(
    x - width / 2,
    total_computation,
    width,
    label="Computation",
    color="steelblue",
    alpha=0.8,
)
bars2 = ax2.bar(
    x + width / 2,
    total_communication,
    width,
    label="Communication",
    color="coral",
    alpha=0.8,
)
ax2.set_xlabel("Scenario")
ax2.set_ylabel("Time Units")
ax2.set_title("Time Breakdown: Computation vs Communication")
ax2.set_xticks(x)
ax2.set_xticklabels([f"{t} tasks" for t in task_counts], rotation=45, ha="right")
ax2.legend()
ax2.grid(True, axis="y", ls="--", alpha=0.7)

# Plot 3: Total Execution Time
ax3 = axes[1, 0]
ax3.plot(task_counts, total_times, "g-s", linewidth=2, markersize=8)
ax3.axhline(y=total_work, color="b", linestyle="--", label="Computation only")
ax3.set_xlabel("Number of Tasks")
ax3.set_ylabel("Total Execution Time")
ax3.set_title("Total Execution Time vs Task Count")
ax3.set_xscale("log")
ax3.legend()
ax3.grid(True, which="both", ls="--", alpha=0.7)

# Plot 4: Efficiency
ax4 = axes[1, 1]
ax4.plot(task_counts, efficiency * 100, "m-d", linewidth=2, markersize=8)
ax4.axhline(y=100, color="g", linestyle="--", label="100% efficiency")
ax4.set_xlabel("Number of Tasks")
ax4.set_ylabel("Efficiency (%)")
ax4.set_title("Efficiency vs Task Count")
ax4.set_xscale("log")
ax4.set_ylim(0, 105)
ax4.legend()
ax4.grid(True, which="both", ls="--", alpha=0.7)

# Add annotation text
fig.text(
    0.5,
    0.02,
    "Fine-grained: Many small tasks, high communication overhead\n"
    "Coarse-grained: Few large tasks, low communication overhead, potential load imbalance",
    ha="center",
    fontsize=10,
    style="italic",
)

plt.tight_layout(rect=[0, 0.05, 1, 1])
plt.show()
