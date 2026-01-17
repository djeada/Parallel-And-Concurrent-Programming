"""
Scatter and Gather Operations

This example demonstrates MPI Scatter and Gather, which are fundamental
collective operations for distributing and collecting data.

Key concepts:
- comm.Scatter: Root distributes equal chunks of data to all processes
- comm.Gather: Root collects equal chunks of data from all processes
- Uses numpy arrays for efficient data transfer (uppercase methods)
- Each process receives/sends an equal portion

Scatter divides an array into equal parts and sends one part to each process.
Gather collects data from all processes into a single array at root.

Run: mpirun -np 4 python scatter_and_gather.py
"""

from mpi4py import MPI
import numpy as np


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    root = 0
    elements_per_process = 3

    # Root process creates the data to scatter
    if rank == root:
        # Create array with size * elements_per_process elements
        total_elements = size * elements_per_process
        data = np.arange(total_elements, dtype='i')
        print(f"Root process: Original data = {data}")
        print(f"Root process: Scattering {elements_per_process} elements to each of {size} processes")
        print()
    else:
        data = None

    # Prepare receive buffer for scattered data
    recv_buffer = np.empty(elements_per_process, dtype='i')

    # Scatter: distribute chunks to all processes
    comm.Scatter(data, recv_buffer, root=root)
    print(f"Process {rank}: Received {recv_buffer}")

    # Each process performs some computation on its data
    recv_buffer = recv_buffer * 2
    print(f"Process {rank}: Processed (x2) = {recv_buffer}")

    # Prepare gather buffer on root
    if rank == root:
        gathered_data = np.empty(size * elements_per_process, dtype='i')
    else:
        gathered_data = None

    # Gather: collect results back to root
    comm.Gather(recv_buffer, gathered_data, root=root)

    # Root displays the gathered results
    if rank == root:
        print()
        print(f"Root process: Gathered data = {gathered_data}")


if __name__ == "__main__":
    main()
