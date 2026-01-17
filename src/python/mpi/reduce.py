"""
Reduce Operation

This example demonstrates MPI reduce, a collective operation that combines
values from all processes using an operation (sum, max, min, etc.) and
returns the result to one process (the root).

Key concepts:
- comm.reduce(data, op, root): Reduces Python objects to root
- comm.Reduce(sendbuf, recvbuf, op, root): Reduces numpy arrays (faster)
- MPI.SUM, MPI.MAX, MPI.MIN, MPI.PROD: Built-in reduction operations
- Only the root process receives the final result

Common reduction operations:
- MPI.SUM: Sum of all values
- MPI.PROD: Product of all values
- MPI.MAX: Maximum value
- MPI.MIN: Minimum value
- MPI.LAND: Logical AND
- MPI.LOR: Logical OR

Run: mpirun -np 4 python reduce.py

With 4 processes, sum = 1+2+3+4 = 10, max = 4, min = 1
"""

from mpi4py import MPI
import numpy as np


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    root = 0

    # Each process has a local value
    local_value = rank + 1
    print(f"Process {rank}: local value = {local_value}")

    # Reduce with SUM operation (using Python objects)
    global_sum = comm.reduce(local_value, op=MPI.SUM, root=root)

    # Reduce with MAX operation
    global_max = comm.reduce(local_value, op=MPI.MAX, root=root)

    # Reduce with MIN operation
    global_min = comm.reduce(local_value, op=MPI.MIN, root=root)

    # Only root has the final results
    if rank == root:
        print()
        print("=== Reduction Results (Python objects) ===")
        print(f"Sum of all values: {global_sum}")
        print(f"Maximum value: {global_max}")
        print(f"Minimum value: {global_min}")

    # Demonstrate with numpy arrays (uppercase Reduce is faster)
    local_array = np.array([rank * 10.0, rank * 20.0, rank * 30.0])
    print(f"Process {rank}: local array = {local_array}")

    if rank == root:
        global_array_sum = np.zeros(3)
    else:
        global_array_sum = None

    # Reduce arrays element-wise
    comm.Reduce(local_array, global_array_sum, op=MPI.SUM, root=root)

    if rank == root:
        print()
        print("=== Array Reduction Results ===")
        print(f"Sum of arrays: {global_array_sum}")


if __name__ == "__main__":
    main()
