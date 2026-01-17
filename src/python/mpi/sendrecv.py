"""
Sendrecv Operation

This example demonstrates comm.sendrecv(), which combines a send and receive
into a single call. This is particularly useful for avoiding deadlocks
in exchange patterns where processes need to both send and receive.

Key concepts:
- comm.sendrecv(sendbuf, dest, source): Atomic send and receive
- Deadlock avoidance: No risk of circular wait
- Can send to one process and receive from a different one
- Useful for neighbor exchange patterns

Problem with separate send/recv:
    Process 0: send to 1, then recv from 1  # DEADLOCK!
    Process 1: send to 0, then recv from 0  # Both waiting to send

Solution with sendrecv:
    Both processes: sendrecv(to_neighbor, from_neighbor)  # Safe!

Run: mpirun -np 4 python sendrecv.py
"""

from mpi4py import MPI


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    if size < 2:
        if rank == 0:
            print("Error: This program requires at least 2 processes.")
        return

    # Example 1: Pairwise exchange
    # Even ranks exchange with rank+1, odd ranks exchange with rank-1
    if rank < size - (size % 2):  # Ensure we have pairs
        partner = rank + 1 if rank % 2 == 0 else rank - 1

        send_data = rank * 100
        print(f"Process {rank}: Exchanging with process {partner}, sending {send_data}")

        recv_data = comm.sendrecv(send_data, dest=partner, source=partner)

        print(f"Process {rank}: Received {recv_data} from process {partner}")

    comm.Barrier()

    # Example 2: Ring shift (each process sends to next, receives from previous)
    if rank == 0:
        print()
        print("=== Ring Shift ===")

    comm.Barrier()

    left = (rank - 1 + size) % size   # Previous in ring
    right = (rank + 1) % size         # Next in ring

    my_value = rank
    # Send my value to the right, receive from the left
    received = comm.sendrecv(my_value, dest=right, source=left)

    print(f"Process {rank}: Sent {my_value} to {right}, received {received} from {left}")


if __name__ == "__main__":
    main()
