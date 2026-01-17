"""
Barrier Synchronization Example

This script demonstrates the use of threading.Barrier to synchronize multiple
threads at a specific point in their execution. All threads must reach the
barrier before any can proceed.

Key Concepts:
- Barrier(n) blocks until n threads call wait()
- Once all threads arrive, they are all released simultaneously
- Useful for parallel algorithms with phases (all must complete phase N before phase N+1)

Use Cases:
- Parallel tasks with dependencies between phases
- Ensuring resources are loaded before processing begins
- Coordinating parallel initialization
- Multi-stage parallel algorithms

Alternative Approaches:
- Condition variables (more complex but flexible)
- Semaphores (for counting-based synchronization)
- Events (for one-time signaling)
"""

import threading
import time
import random


def worker(barrier, thread_id):
    """
    Worker function that simulates work and synchronizes at a barrier.

    Each worker:
    1. Does some initial work (simulated by random sleep)
    2. Waits at the barrier for all other workers
    3. Continues with post-barrier work
    """
    print(f"Thread {thread_id} is starting...")
    time.sleep(random.uniform(1, 3))  # Simulate variable work time
    print(f"Thread {thread_id} is waiting at the barrier...")

    barrier.wait()  # Block until all threads reach this point

    print(f"Thread {thread_id} is resuming after the barrier...")


def main():
    num_threads = 5
    barrier = threading.Barrier(num_threads)

    threads = [
        threading.Thread(target=worker, args=(barrier, i)) for i in range(num_threads)
    ]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print("All threads have passed the barrier.")


if __name__ == "__main__":
    main()
