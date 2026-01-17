"""
Non-Blocking Communication

This example demonstrates non-blocking communication using isend()
and irecv(). Unlike blocking operations, these return immediately,
allowing computation to overlap with communication.

Key concepts:
- comm.isend(data, dest, tag): Non-blocking send, returns Request
- comm.irecv(source, tag): Non-blocking receive, returns Request
- request.wait(): Blocks until the operation completes
- request.test(): Checks if operation completed (non-blocking check)
- MPI.Request.Waitall(): Waits for multiple operations

Benefits of non-blocking communication:
- Overlap computation with communication
- Avoid deadlocks in complex communication patterns
- Better performance in many scenarios

Run: mpirun -np 2 python non_blocking_communication.py
"""

from mpi4py import MPI
import time


def do_computation(rank: int) -> int:
    """Simulate useful work during communication."""
    print(f"Process {rank}: Performing computation while waiting...")
    total = 0
    for i in range(1000000):
        total += i
    return total


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    if size < 2:
        if rank == 0:
            print("Error: This program requires at least 2 processes.")
        return

    TAG = 0

    if rank == 0:
        data = {"message": "Hello from process 0", "value": 42}
        print(f"Process 0: Initiating non-blocking send of {data}")

        # Start non-blocking send - returns immediately
        request = comm.isend(data, dest=1, tag=TAG)

        # Do useful work while message is being sent
        result = do_computation(rank)
        print(f"Process 0: Computation result = {result}")

        # Wait for send to complete
        request.wait()
        print("Process 0: Send completed")

    elif rank == 1:
        print("Process 1: Initiating non-blocking receive")

        # Start non-blocking receive - returns immediately
        request = comm.irecv(source=0, tag=TAG)

        # Do useful work while waiting for message
        result = do_computation(rank)
        print(f"Process 1: Computation result = {result}")

        # Wait for receive to complete and get data
        data = request.wait()
        print(f"Process 1: Received {data}")

    # Demonstrate using test() to check completion
    comm.Barrier()

    if rank == 0:
        print()
        print("=== Demonstrating test() ===")
        data = "Test message"
        request = comm.isend(data, dest=1, tag=1)

        # Poll until complete
        while True:
            completed, _ = request.test()
            if completed:
                print("Process 0: Send completed (via test)")
                break
            print("Process 0: Send not yet complete, doing other work...")
            time.sleep(0.01)

    elif rank == 1:
        request = comm.irecv(source=0, tag=1)

        while True:
            completed, data = request.test()
            if completed:
                print(f"Process 1: Received '{data}' (via test)")
                break
            print("Process 1: Receive not yet complete, doing other work...")
            time.sleep(0.01)


if __name__ == "__main__":
    main()
