"""
Broadcast Communication

This example demonstrates MPI broadcast (bcast), a collective operation that
sends data from one process (the root) to all other processes in a communicator.

Key concepts:
- comm.bcast(data, root): Broadcasts Python objects from root to all
- All processes must call bcast (it's a collective operation)
- After the call, all processes have the same data
- Uses Python's pickle for serialization (lowercase method)

Broadcast is commonly used to distribute:
- Configuration parameters
- Initial data or settings
- Shared constants

Run: mpirun -np 4 python broadcast.py
"""

from mpi4py import MPI


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    root = 0

    # Root process initializes the data to broadcast
    if rank == root:
        data = {
            "config": "production",
            "max_iterations": 1000,
            "tolerance": 1e-6,
            "parameters": [1.0, 2.0, 3.0]
        }
        print(f"Process {rank} (root): Broadcasting data: {data}")
    else:
        data = None
        print(f"Process {rank}: Waiting to receive broadcast...")

    # All processes call bcast - this is a collective operation
    data = comm.bcast(data, root=root)

    # After broadcast, all processes have the same data
    print(f"Process {rank}: Received data: {data}")

    # Each process can now use the broadcasted data
    max_iter = data["max_iterations"]
    tolerance = data["tolerance"]
    print(f"Process {rank}: Using max_iterations={max_iter}, tolerance={tolerance}")


if __name__ == "__main__":
    main()
