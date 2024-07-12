import matplotlib.pyplot as plt
import numpy as np

def amdahl_speedup(P, N):
    return 1 / ((1 - P) + (P / N))

# Number of processors
processors = np.logspace(0, 16, num=17, base=2)

# Parallel portions
P_75 = 0.75
P_90 = 0.90
P_95 = 0.95

# Calculate speedups
speedup_75 = [amdahl_speedup(P_75, n) for n in processors]
speedup_90 = [amdahl_speedup(P_90, n) for n in processors]
speedup_95 = [amdahl_speedup(P_95, n) for n in processors]

# Plotting
plt.figure(figsize=(10, 7))
plt.plot(processors, speedup_75, 'y--', label='75%')
plt.plot(processors, speedup_90, 'k--', label='90%')
plt.plot(processors, speedup_95, 'g--', label='95%')

plt.xscale('log', base=2)
plt.yscale('linear')
plt.xlabel('Number of processors')
plt.ylabel('Speedup')
plt.title('Speedup vs Number of Processors (Amdahl\'s Law)')
plt.legend(title='Parallel portion')
plt.grid(True, which="both", ls="--")

plt.show()
