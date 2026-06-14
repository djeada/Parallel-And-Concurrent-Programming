"""
Process Semaphore Example

This script demonstrates using multiprocessing.Semaphore to control concurrent
access to a resource by limiting the number of processes that can access it
simultaneously.

Key Concepts:
- Semaphore(N) allows up to N processes in the critical section
- Use 'with semaphore:' for automatic acquire/release
- Useful for connection pools, rate limiting, resource pools
- Prefer Lock for ordinary mutual exclusion; use Semaphore(N) for capacity limits

Use Cases:
- Limiting concurrent database connections
- Controlling access to APIs with rate limits
- Managing access to limited hardware resources
- Implementing worker pools with bounded concurrency

Note: This example passes the Semaphore explicitly to child processes, which
works with both fork and spawn start methods when guarded by
if __name__ == "__main__".
"""

import multiprocessing
import time
import random


def shared_resource(process_id):
    """Simulate using a shared resource."""
    print(f"Process {process_id} is using the shared resource", flush=True)
    time.sleep(random.uniform(0.5, 1))
    print(f"Process {process_id} is done using the shared resource", flush=True)


def worker(semaphore, process_id):
    """Worker that respects the semaphore limit."""
    print(f"Process {process_id} is waiting for the semaphore", flush=True)
    with semaphore:
        print(f"Process {process_id} acquired the semaphore", flush=True)
        shared_resource(process_id)
        print(f"Process {process_id} released the semaphore", flush=True)


def main():
    num_processes = 10
    max_concurrent = 3  # Maximum concurrent accesses

    # Create semaphore allowing up to max_concurrent processes
    semaphore = multiprocessing.Semaphore(max_concurrent)

    # Create and start worker processes
    processes = [
        multiprocessing.Process(target=worker, args=(semaphore, i))
        for i in range(num_processes)
    ]

    for process in processes:
        process.start()

    for process in processes:
        process.join()


if __name__ == "__main__":
    main()
