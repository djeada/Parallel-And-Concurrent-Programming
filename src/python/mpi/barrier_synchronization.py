"""
Barrier Synchronization

This example demonstrates MPI barrier, which synchronizes all processes
in a communicator. No process can proceed past the barrier until all
processes have reached it.

Key concepts:
- comm.Barrier(): Blocks until all processes in the communicator call it
- Synchronization point: Ensures all processes are at the same stage
- Use sparingly: Barriers can hurt performance if overused

Common use cases:
- Ensuring all processes have completed a phase before starting the next
- Synchronizing before timing measurements
- Ensuring file I/O is complete before reading results

Note: Many collective operations (bcast, reduce, etc.) have implicit
synchronization, so explicit barriers are often unnecessary.

Run: mpirun -np 4 python barrier_synchronization.py
"""

from mpi4py import MPI
import time


def do_work(rank: int) -> None:
    """Simulate variable work with different durations."""
    # Each process works for a different amount of time
    sleep_time = (rank + 1) * 0.2  # 0.2s, 0.4s, 0.6s, ...
    time.sleep(sleep_time)


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # Phase 1: Each process does variable-length work
    print(f"Process {rank}: Starting Phase 1 work...")
    do_work(rank)
    print(f"Process {rank}: Phase 1 complete")

    # Barrier: Wait for all processes to complete Phase 1
    print(f"Process {rank}: Waiting at barrier...")
    comm.Barrier()
    print(f"Process {rank}: Passed barrier")

    # Phase 2: All processes start together
    if rank == 0:
        print()
        print("=== All processes synchronized ===")
        print("=== Starting Phase 2 ===")
        print()

    # Another barrier to ensure the message is printed before Phase 2 output
    comm.Barrier()

    print(f"Process {rank}: Phase 2 starting now")

    # Demonstrate timing with barrier synchronization
    comm.Barrier()
    start_time = MPI.Wtime()

    # All processes do the same work
    do_work(0)  # Same duration for all

    end_time = MPI.Wtime()
    elapsed = end_time - start_time

    print(f"Process {rank}: Elapsed time = {elapsed:.3f} seconds")

    # Final barrier before exit
    comm.Barrier()

    if rank == 0:
        print()
        print("All processes completed successfully")


if __name__ == "__main__":
    main()
