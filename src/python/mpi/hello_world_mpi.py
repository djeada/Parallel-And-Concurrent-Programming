"""
Hello World MPI - Basic Initialization

This example demonstrates the fundamental structure of an MPI program using mpi4py.
Every MPI program needs to initialize the communication environment and can query
basic information about the parallel execution context.

Key concepts:
- MPI.COMM_WORLD: Default communicator containing all processes
- comm.Get_rank(): Gets the rank (unique ID) of the calling process
- comm.Get_size(): Gets the total number of processes
- Ranks are numbered from 0 to size-1

Run: mpirun -np 4 python hello_world_mpi.py

Expected output (order may vary):
    Hello from rank 0 out of 4 processes
    Hello from rank 1 out of 4 processes
    Hello from rank 2 out of 4 processes
    Hello from rank 3 out of 4 processes
"""

from mpi4py import MPI


def main():
    # Get the default communicator (contains all processes)
    comm = MPI.COMM_WORLD

    # Get the rank (unique identifier) of this process
    rank = comm.Get_rank()

    # Get the total number of processes in the communicator
    size = comm.Get_size()

    # Each process prints its greeting
    print(f"Hello from rank {rank} out of {size} processes")

    # Demonstrate that each process has its own memory space
    local_value = rank * 10
    print(f"Process {rank}: local_value = {local_value}")


if __name__ == "__main__":
    main()
