import matplotlib.pyplot as plt
import numpy as np


def gustafson_speedup(s, N):
    """
    Calculate speedup according to Gustafson's Law.

    Gustafson's Law provides an alternative to Amdahl's Law by considering
    scaled speedup, where the problem size grows with the number of processors.

    Parameters:
    - s: Serial fraction (proportion of work that must be done sequentially)
    - N: Number of processors

    Returns:
    - Scaled speedup
    """
    return N - s * (N - 1)


# Number of processors
processors = np.arange(1, 65)

# Serial fractions
s_05 = 0.05  # 5% serial
s_10 = 0.10  # 10% serial
s_25 = 0.25  # 25% serial

# Calculate speedups
speedup_05 = [gustafson_speedup(s_05, n) for n in processors]
speedup_10 = [gustafson_speedup(s_10, n) for n in processors]
speedup_25 = [gustafson_speedup(s_25, n) for n in processors]

# Plotting
plt.figure(figsize=(10, 7))
plt.plot(processors, speedup_05, "g-", linewidth=2, label="5% serial")
plt.plot(processors, speedup_10, "b-", linewidth=2, label="10% serial")
plt.plot(processors, speedup_25, "r-", linewidth=2, label="25% serial")
plt.plot(processors, processors, "k--", linewidth=1, label="Linear speedup")

plt.xlabel("Number of processors")
plt.ylabel("Scaled Speedup")
plt.title("Gustafson's Law: Scaled Speedup vs Number of Processors")
plt.legend(title="Serial fraction")
plt.grid(True, which="both", ls="--", alpha=0.7)
plt.xlim(1, 64)
plt.ylim(0, 70)

plt.tight_layout()
plt.show()
