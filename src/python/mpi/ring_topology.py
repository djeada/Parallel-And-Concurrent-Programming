"""
Ring Topology Communication

This example demonstrates a ring communication pattern where each
process passes data to its neighbor in a circular fashion. This is
a fundamental pattern in many parallel algorithms.

Key concepts:
- Ring topology: Processes arranged in a circle (0->1->2->...->0)
- Token passing: Data circulates through all processes
- Neighbor calculation: left = (rank - 1 + size) % size
                       right = (rank + 1) % size

Applications:
- Token ring algorithms
- Pipeline computations
- Circular data redistribution
- Some collective operation implementations

Pattern:
    Process 0 -> Process 1 -> Process 2 -> ... -> Process 0

Run: mpirun -np 4 python ring_topology.py
"""

from mpi4py import MPI


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # Calculate neighbors in the ring
    left = (rank - 1 + size) % size
    right = (rank + 1) % size

    print(f"Process {rank}: left neighbor = {left}, right neighbor = {right}")

    comm.Barrier()

    # Example 1: Simple token passing around the ring
    if rank == 0:
        print()
        print("=== Token Passing ===")

    comm.Barrier()

    TAG = 0

    if rank == 0:
        # Process 0 starts the token
        token = 42
        print(f"Process 0: Starting token with value {token}")
        comm.send(token, dest=right, tag=TAG)
        token = comm.recv(source=left, tag=TAG)
        print(f"Process 0: Token returned with value {token}")
    else:
        # Other processes receive, modify, and pass the token
        token = comm.recv(source=left, tag=TAG)
        print(f"Process {rank}: Received token with value {token}")
        token += rank  # Each process adds its rank
        print(f"Process {rank}: Passing token with value {token}")
        comm.send(token, dest=right, tag=TAG)

    comm.Barrier()

    # Example 2: Ring allgather - each process shares data with all others
    if rank == 0:
        print()
        print("=== Ring Allgather ===")

    comm.Barrier()

    # Each process has some local data
    my_data = rank * 100
    all_data = [None] * size
    all_data[rank] = my_data

    print(f"Process {rank}: my_data = {my_data}")

    # Pass data around the ring, accumulating all values
    send_data = my_data
    for step in range(size - 1):
        recv_data = comm.sendrecv(send_data, dest=right, source=left, sendtag=TAG, recvtag=TAG)

        # Store received data in the correct position
        source_rank = (rank - step - 1 + size) % size
        all_data[source_rank] = recv_data

        # Pass along what we received
        send_data = recv_data

    # Each process now has all data
    print(f"Process {rank}: all_data = {all_data}")


if __name__ == "__main__":
    main()
