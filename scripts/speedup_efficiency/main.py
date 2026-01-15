import matplotlib.pyplot as plt
import numpy as np


def calculate_speedup(T_1, T_p):
    """
    Calculate speedup as the ratio of sequential time to parallel time.

    Parameters:
    - T_1: Time with one processor
    - T_p: Time with p processors

    Returns:
    - Speedup value
    """
    return T_1 / T_p


def calculate_efficiency(speedup, p):
    """
    Calculate efficiency as speedup divided by the number of processors.

    Parameters:
    - speedup: The speedup achieved
    - p: Number of processors

    Returns:
    - Efficiency value (0 to 1, where 1 is 100% efficient)
    """
    return speedup / p


def amdahl_speedup(P, N):
    """
    Calculate speedup according to Amdahl's Law.

    Parameters:
    - P: Parallel fraction (proportion that can be parallelized)
    - N: Number of processors

    Returns:
    - Speedup value
    """
    return 1 / ((1 - P) + (P / N))


# Number of processors
processors = np.arange(1, 65)

# Parallel portions
P_values = [0.50, 0.75, 0.90, 0.95, 0.99]
colors = ["purple", "blue", "green", "orange", "red"]
labels = ["50%", "75%", "90%", "95%", "99%"]

# Create subplots
fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# Plot 1: Speedup
ax1 = axes[0]
for P, color, label in zip(P_values, colors, labels):
    speedups = [amdahl_speedup(P, n) for n in processors]
    ax1.plot(processors, speedups, color=color, linewidth=2, label=f"{label} parallel")

ax1.plot(processors, processors, "k--", linewidth=1, label="Linear speedup")
ax1.set_xlabel("Number of processors")
ax1.set_ylabel("Speedup")
ax1.set_title("Speedup vs Number of Processors")
ax1.legend(title="Parallel portion")
ax1.grid(True, which="both", ls="--", alpha=0.7)
ax1.set_xlim(1, 64)
ax1.set_ylim(0, 70)

# Plot 2: Efficiency
ax2 = axes[1]
for P, color, label in zip(P_values, colors, labels):
    speedups = [amdahl_speedup(P, n) for n in processors]
    efficiencies = [calculate_efficiency(s, n) for s, n in zip(speedups, processors)]
    ax2.plot(
        processors, efficiencies, color=color, linewidth=2, label=f"{label} parallel"
    )

ax2.axhline(y=1.0, color="k", linestyle="--", linewidth=1, label="100% efficiency")
ax2.set_xlabel("Number of processors")
ax2.set_ylabel("Efficiency")
ax2.set_title("Efficiency vs Number of Processors")
ax2.legend(title="Parallel portion")
ax2.grid(True, which="both", ls="--", alpha=0.7)
ax2.set_xlim(1, 64)
ax2.set_ylim(0, 1.1)

plt.tight_layout()
plt.show()
