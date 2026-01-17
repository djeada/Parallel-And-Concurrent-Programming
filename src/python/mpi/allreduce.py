"""
Allreduce Operation

This example demonstrates MPI allreduce, which performs a reduction
operation and broadcasts the result to ALL processes (unlike reduce
which only sends the result to the root).

Key concepts:
- comm.allreduce(data, op): Reduces and broadcasts to all (Python objects)
- comm.Allreduce(sendbuf, recvbuf, op): For numpy arrays (faster)
- Equivalent to reduce + broadcast, but more efficient
- All processes have the same result after the call

Use cases:
- Global convergence checks in iterative algorithms
- Computing global statistics needed by all processes
- Normalization factors in parallel algorithms

Common pattern in iterative solvers:
    while global_error > tolerance:
        compute_local_error()
        global_error = comm.allreduce(local_error, op=MPI.MAX)

Run: mpirun -np 4 python allreduce.py
"""

from mpi4py import MPI
import numpy as np


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # Each process has local data
    local_value = float(rank + 1)
    print(f"Process {rank}: local value = {local_value}")

    # Allreduce with SUM - all processes get the sum
    global_sum = comm.allreduce(local_value, op=MPI.SUM)
    print(f"Process {rank}: global sum = {global_sum}")

    # Compute local contribution as percentage of total
    local_percentage = (local_value / global_sum) * 100.0
    print(f"Process {rank}: contributes {local_percentage:.1f}% of total")

    # Demonstrate with numpy arrays for computing statistics
    local_data = np.array([rank * 10 + 1, rank * 10 + 2, rank * 10 + 3], dtype='d')
    print(f"Process {rank}: local data = {local_data}")

    # Compute global sum of all data
    local_sum = np.sum(local_data)
    total_sum = comm.allreduce(local_sum, op=MPI.SUM)

    # Compute global mean
    total_elements = len(local_data) * size
    global_mean = total_sum / total_elements
    print(f"Process {rank}: global mean = {global_mean:.2f}")

    # Compute variance using global mean
    local_variance_sum = np.sum((local_data - global_mean) ** 2)
    global_variance_sum = comm.allreduce(local_variance_sum, op=MPI.SUM)
    global_std_dev = np.sqrt(global_variance_sum / total_elements)

    if rank == 0:
        print()
        print("=== Global Statistics ===")
        print(f"Total sum: {total_sum}")
        print(f"Mean: {global_mean:.2f}")
        print(f"Standard deviation: {global_std_dev:.2f}")


if __name__ == "__main__":
    main()
