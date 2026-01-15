import matplotlib.pyplot as plt
import numpy as np


def compute_parallel_time(
    compute_time, comm_time, num_procs, overlap_factor=0.0
):
    """
    Compute total parallel execution time including communication overhead.

    Parameters:
    - compute_time: Pure computation time per processor
    - comm_time: Communication time per processor
    - num_procs: Number of processors
    - overlap_factor: Fraction of communication that can overlap with computation (0 to 1)

    Returns:
    - Total execution time
    """
    effective_comm = comm_time * (1 - overlap_factor)
    return compute_time + effective_comm


def compute_speedup(serial_time, parallel_time):
    """Calculate speedup from serial and parallel times."""
    return serial_time / parallel_time


# Simulation parameters
total_work = 10000  # Total computation units
base_serial_time = total_work

# Number of processors range
num_processors = np.array([1, 2, 4, 8, 16, 32, 64, 128])

# Different communication models
# Alpha-Beta model: T_comm = alpha + beta * message_size
alpha = 10  # Latency (startup cost)
message_sizes = [100, 500, 1000, 5000]  # Different message sizes
beta = 0.01  # Transfer time per unit

# Calculate metrics for point-to-point communication
fig, axes = plt.subplots(2, 2, figsize=(14, 10))

# Plot 1: Speedup with different message sizes
ax1 = axes[0, 0]
colors = ["green", "blue", "orange", "red"]

for msg_size, color in zip(message_sizes, colors):
    speedups = []
    for p in num_processors:
        compute_per_proc = total_work / p
        # Point-to-point communication: each proc sends to neighbor
        comm_time = alpha + beta * msg_size
        parallel_time = compute_per_proc + comm_time
        speedup = base_serial_time / parallel_time
        speedups.append(speedup)
    ax1.plot(
        num_processors, speedups, "-o", color=color, linewidth=2, label=f"msg={msg_size}"
    )

# Ideal speedup
ax1.plot(num_processors, num_processors, "k--", linewidth=1.5, label="Ideal")

ax1.set_xlabel("Number of Processors")
ax1.set_ylabel("Speedup")
ax1.set_title("Speedup vs Processors (Point-to-Point Communication)")
ax1.legend(title="Message Size")
ax1.grid(True, which="both", ls="--", alpha=0.7)
ax1.set_xscale("log", base=2)
ax1.set_yscale("log", base=2)

# Plot 2: Time breakdown
ax2 = axes[0, 1]
selected_procs = [2, 8, 32, 128]
x = np.arange(len(selected_procs))
width = 0.2

msg_size = 1000
for i, p in enumerate(selected_procs):
    compute = total_work / p
    comm = alpha + beta * msg_size
    ax2.bar(
        x[i] - width, compute, width * 2, label="Computation" if i == 0 else "", color="steelblue"
    )
    ax2.bar(
        x[i] + width, comm, width * 2, label="Communication" if i == 0 else "", color="coral"
    )

ax2.set_xlabel("Number of Processors")
ax2.set_ylabel("Time Units")
ax2.set_title(f"Time Breakdown (Message Size = {msg_size})")
ax2.set_xticks(x)
ax2.set_xticklabels([str(p) for p in selected_procs])
ax2.legend()
ax2.grid(True, axis="y", ls="--", alpha=0.7)

# Plot 3: Collective Communication Patterns
ax3 = axes[1, 0]

# Different collective patterns have different scaling
# Broadcast: O(log p)
# Allreduce: O(log p)
# Alltoall: O(p)

def broadcast_time(p, msg_size, alpha, beta):
    return np.log2(p) * (alpha + beta * msg_size)


def allreduce_time(p, msg_size, alpha, beta):
    return 2 * np.log2(p) * (alpha + beta * msg_size)


def alltoall_time(p, msg_size, alpha, beta):
    return (p - 1) * (alpha + beta * msg_size / p)


msg_size = 1000
procs = np.array([2, 4, 8, 16, 32, 64])

broadcast_times = [broadcast_time(p, msg_size, alpha, beta) for p in procs]
allreduce_times = [allreduce_time(p, msg_size, alpha, beta) for p in procs]
alltoall_times = [alltoall_time(p, msg_size, alpha, beta) for p in procs]

ax3.plot(procs, broadcast_times, "g-^", linewidth=2, markersize=8, label="Broadcast O(log p)")
ax3.plot(procs, allreduce_times, "b-s", linewidth=2, markersize=8, label="Allreduce O(log p)")
ax3.plot(procs, alltoall_times, "r-o", linewidth=2, markersize=8, label="All-to-All O(p)")

ax3.set_xlabel("Number of Processors")
ax3.set_ylabel("Communication Time")
ax3.set_title("Collective Communication Scaling")
ax3.legend()
ax3.grid(True, which="both", ls="--", alpha=0.7)
ax3.set_xscale("log", base=2)

# Plot 4: Communication to Computation Ratio
ax4 = axes[1, 1]

msg_size = 1000
comm_compute_ratios = []
efficiency_values = []

for p in num_processors:
    compute = total_work / p
    comm = alpha + beta * msg_size
    ratio = comm / compute
    eff = compute / (compute + comm)
    comm_compute_ratios.append(ratio * 100)  # as percentage
    efficiency_values.append(eff * 100)

ax4.plot(
    num_processors,
    comm_compute_ratios,
    "r-o",
    linewidth=2,
    markersize=8,
    label="Comm/Compute Ratio",
)
ax4.plot(
    num_processors,
    efficiency_values,
    "g-s",
    linewidth=2,
    markersize=8,
    label="Efficiency",
)

ax4.axhline(y=50, color="orange", linestyle="--", alpha=0.7, label="50% threshold")
ax4.set_xlabel("Number of Processors")
ax4.set_ylabel("Percentage (%)")
ax4.set_title("Communication Overhead Impact")
ax4.legend()
ax4.grid(True, which="both", ls="--", alpha=0.7)
ax4.set_xscale("log", base=2)

plt.tight_layout()
plt.show()
