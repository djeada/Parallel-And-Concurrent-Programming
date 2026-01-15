import matplotlib.pyplot as plt
import numpy as np


def roofline_performance(operational_intensity, peak_flops, peak_bandwidth):
    """
    Calculate achievable performance using the Roofline model.

    The Roofline model bounds performance by either:
    - Memory bandwidth (memory-bound region)
    - Peak compute throughput (compute-bound region)

    Parameters:
    - operational_intensity: FLOP/Byte ratio
    - peak_flops: Peak computational throughput (GFLOP/s)
    - peak_bandwidth: Peak memory bandwidth (GB/s)

    Returns:
    - Achievable performance (GFLOP/s)
    """
    memory_bound = operational_intensity * peak_bandwidth
    return min(memory_bound, peak_flops)


# Hardware specifications (representative values for educational purposes)
# GPU specs are representative of high-end gaming/compute GPUs (e.g., NVIDIA A100-like)
# CPU specs are representative of high-end server CPUs (e.g., Intel Xeon-like)
peak_flops_gpu = 10000  # GFLOP/s (10 TFLOP/s) - FP32 peak performance
peak_bandwidth_gpu = 900  # GB/s - HBM2e memory bandwidth
peak_flops_cpu = 500  # GFLOP/s - Multi-core server CPU with AVX-512
peak_bandwidth_cpu = 100  # GB/s - DDR4/DDR5 memory bandwidth

# Operational intensity range (FLOP/Byte)
oi = np.logspace(-2, 3, 1000)

# Calculate achievable performance
perf_gpu = [roofline_performance(x, peak_flops_gpu, peak_bandwidth_gpu) for x in oi]
perf_cpu = [roofline_performance(x, peak_flops_cpu, peak_bandwidth_cpu) for x in oi]

# Create figure
fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# Plot 1: Roofline Model Comparison
ax1 = axes[0]

# GPU roofline
ax1.loglog(oi, perf_gpu, "r-", linewidth=2.5, label="GPU Roofline")
# CPU roofline
ax1.loglog(oi, perf_cpu, "b-", linewidth=2.5, label="CPU Roofline")

# Add ridge points
ridge_gpu = peak_flops_gpu / peak_bandwidth_gpu
ridge_cpu = peak_flops_cpu / peak_bandwidth_cpu
ax1.axvline(
    x=ridge_gpu, color="r", linestyle="--", alpha=0.5, label=f"GPU Ridge ({ridge_gpu:.1f})"
)
ax1.axvline(
    x=ridge_cpu, color="b", linestyle="--", alpha=0.5, label=f"CPU Ridge ({ridge_cpu:.1f})"
)

# Mark example applications
applications = {
    "SpMV": (0.25, "Memory-bound"),
    "Stencil": (1.0, "Transition"),
    "Dense MM": (50.0, "Compute-bound"),
    "FFT": (5.0, "Mixed"),
}

for name, (oi_val, desc) in applications.items():
    perf_val = roofline_performance(oi_val, peak_flops_gpu, peak_bandwidth_gpu)
    ax1.scatter([oi_val], [perf_val], s=100, zorder=5)
    ax1.annotate(
        f"{name}\n({desc})",
        (oi_val, perf_val),
        textcoords="offset points",
        xytext=(10, 10),
        fontsize=9,
    )

ax1.set_xlabel("Operational Intensity (FLOP/Byte)")
ax1.set_ylabel("Performance (GFLOP/s)")
ax1.set_title("Roofline Model: GPU vs CPU")
ax1.legend(loc="lower right")
ax1.grid(True, which="both", ls="--", alpha=0.5)
ax1.set_xlim(0.01, 1000)
ax1.set_ylim(1, 20000)

# Shade regions
ax1.fill_between(
    oi[oi < ridge_gpu],
    0.1,
    peak_flops_gpu,
    alpha=0.1,
    color="red",
    label="Memory-bound",
)

# Plot 2: Memory Access Patterns Impact
ax2 = axes[1]

# Simulated bandwidth utilization for different access patterns
# GPU memory coalescing: threads in a warp should access consecutive memory addresses
# Strided access causes multiple memory transactions, reducing effective bandwidth
stride_values = [1, 2, 4, 8, 16, 32, 64, 128]

# Model: Bandwidth efficiency decreases proportionally to stride
# Stride 1 (coalesced) = 100% efficiency
# Stride N = 100/N efficiency (each access fetches a full cache line but uses 1/N)
coalesced_efficiency = [100 / s for s in stride_values]

# Random access is capped at very low efficiency due to cache line waste
# and lack of spatial locality
random_efficiency = [min(e, 5) for e in coalesced_efficiency]

x_pos = np.arange(len(stride_values))
width = 0.35

bars1 = ax2.bar(
    x_pos - width / 2,
    coalesced_efficiency,
    width,
    label="Strided Access",
    color="steelblue",
)
bars2 = ax2.bar(
    x_pos + width / 2, random_efficiency, width, label="Random Access", color="coral"
)

ax2.set_xlabel("Access Stride (elements)")
ax2.set_ylabel("Bandwidth Utilization (%)")
ax2.set_title("Memory Access Pattern Impact on Bandwidth")
ax2.set_xticks(x_pos)
ax2.set_xticklabels(stride_values)
ax2.legend()
ax2.grid(True, axis="y", ls="--", alpha=0.7)

# Add optimal access annotation
ax2.annotate(
    "Coalesced\n(Optimal)",
    xy=(0, 100),
    xytext=(0.5, 80),
    arrowprops=dict(arrowstyle="->", color="green"),
    fontsize=10,
    color="green",
)

plt.tight_layout()
plt.show()
