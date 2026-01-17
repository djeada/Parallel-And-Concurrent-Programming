"""
Process Barrier Example

This script demonstrates using multiprocessing.Barrier to synchronize
multiple processes at a specific point in their execution. All processes
must reach the barrier before any can proceed.

Key Concepts:
- Barrier(N) blocks until N processes call wait()
- Once all processes arrive, they are all released simultaneously
- Useful for parallel algorithms with phases
- Can include an optional action to run when barrier is reached

Use Cases:
- Parallel tasks with dependencies between phases
- Ensuring all workers are initialized before starting
- Coordinating parallel algorithm stages
- Synchronizing distributed simulations

Note: Barrier is ideal when you need all processes to complete a stage
before any can start the next stage.
"""

import multiprocessing
import time
import random


def worker(barrier, worker_id):
    """
    Worker that performs work and synchronizes at a barrier.

    Each worker:
    1. Performs initial work (simulated by random sleep)
    2. Waits at the barrier for all other workers
    3. Continues with post-barrier work
    """
    # Simulate initial work with varying durations
    sleep_time = random.uniform(1, 3)
    time.sleep(sleep_time)
    print(f"Worker {worker_id} finished initial work and is waiting at the barrier...")

    # Wait for all workers to reach the barrier
    barrier.wait()

    print(f"Worker {worker_id} passed the barrier and is continuing execution...")


def main():
    num_workers = 5

    # Create barrier requiring num_workers processes
    barrier = multiprocessing.Barrier(num_workers)

    # Create and start worker processes
    processes = [
        multiprocessing.Process(target=worker, args=(barrier, i))
        for i in range(num_workers)
    ]

    for p in processes:
        p.start()

    for p in processes:
        p.join()

    print("All processes have passed the barrier.")


if __name__ == "__main__":
    main()
