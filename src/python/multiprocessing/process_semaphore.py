"""
Process Semaphore Example

This script demonstrates using multiprocessing.Semaphore to control concurrent
access to a resource by limiting the number of processes that can access it
simultaneously.

Key Concepts:
- Semaphore(N) allows up to N processes in the critical section
- Semaphore(1) behaves like a Lock (binary semaphore)
- Use 'with semaphore:' for automatic acquire/release
- Useful for connection pools, rate limiting, resource pools

Use Cases:
- Limiting concurrent database connections
- Controlling access to APIs with rate limits
- Managing access to limited hardware resources
- Implementing worker pools with bounded concurrency

Note: This example passes the Semaphore directly to child processes, which
works on Unix (fork) but may have issues on Windows (spawn). For cross-platform
production code, use an initializer function to set up global semaphores.
"""

import multiprocessing
import time
import random


def shared_resource(process_id):
    """Simulate using a shared resource."""
    print(f"Process {process_id} is using the shared resource")
    time.sleep(random.uniform(0.5, 1))
    print(f"Process {process_id} is done using the shared resource")


def worker(semaphore, process_id):
    """Worker that respects the semaphore limit."""
    print(f"Process {process_id} is waiting for the semaphore")
    with semaphore:
        print(f"Process {process_id} acquired the semaphore")
        shared_resource(process_id)
        print(f"Process {process_id} released the semaphore")


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
