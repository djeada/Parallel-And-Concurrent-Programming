"""
Parallel Computation of Pi

This example demonstrates a classic parallel computing problem:
computing Pi using numerical integration of the arctangent derivative.
Each process computes a portion of the sum, and results are reduced.

Mathematical basis:
    pi = integral from 0 to 1 of 4/(1+x^2) dx

Key concepts:
- Work distribution: Each process handles every size-th interval
- comm.reduce: Combines partial sums from all processes
- Midpoint rule: Uses (i + 0.5) * h for better accuracy

This is an embarrassingly parallel problem - no communication
between processes during computation, only at the end for reduction.

Run: mpirun -np 4 python parallel_computation_of_pi.py

Expected output: Pi ≈ 3.14159265358979...
"""

from mpi4py import MPI
import math


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # Number of intervals for numerical integration
    n = 10_000_000
    h = 1.0 / n

    # Each process computes its portion of the sum
    # Process i handles intervals: i, i+size, i+2*size, ...
    local_sum = 0.0
    for i in range(rank, n, size):
        x = h * (i + 0.5)  # Midpoint
        local_sum += 4.0 / (1.0 + x * x)

    local_sum *= h

    # Report local contribution
    print(f"Process {rank}: local_sum = {local_sum:.10f}")

    # Reduce all local sums to get pi
    pi = comm.reduce(local_sum, op=MPI.SUM, root=0)

    # Root prints the result
    if rank == 0:
        print()
        print("=== Results ===")
        print(f"Computed Pi: {pi:.15f}")
        print(f"Reference:   {math.pi:.15f}")
        print(f"Error:       {abs(pi - math.pi):.2e}")
        print(f"Intervals:   {n}")
        print(f"Processes:   {size}")


if __name__ == "__main__":
    main()
