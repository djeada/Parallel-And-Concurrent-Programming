"""
Process Lock (Mutex) Example

This script demonstrates using multiprocessing.Lock to synchronize access
to shared resources across multiple processes. A lock ensures that only
one process can access a critical section at a time.

Key Concepts:
- Lock provides mutual exclusion (mutex) for processes
- Use 'with lock:' for automatic acquire/release
- Prevents race conditions when modifying shared data
- Essential when multiple processes access the same resource

Use Cases:
- Protecting shared counters or variables
- Coordinating access to files or external resources
- Ensuring atomic read-modify-write operations

Note: Unlike threading.Lock, multiprocessing.Lock works across processes
by using OS-level synchronization primitives.
"""

import multiprocessing
import time


def worker(mutex, counter):
    """Worker that safely increments a shared counter."""
    with mutex:
        print(f"Process {multiprocessing.current_process().name} is starting...")
        time.sleep(1)  # Simulate work while holding the lock
        counter.value += 1
        print(
            f"Process {multiprocessing.current_process().name} is finished. "
            f"Counter: {counter.value}"
        )


def main():
    num_processes = 4

    # Create shared counter and lock
    counter = multiprocessing.Value("i", 0)
    mutex = multiprocessing.Lock()

    # Create worker processes
    processes = [
        multiprocessing.Process(target=worker, args=(mutex, counter))
        for _ in range(num_processes)
    ]

    # Start all processes
    for process in processes:
        process.start()

    # Wait for all processes to complete
    for process in processes:
        process.join()

    print(f"Final counter value: {counter.value}")


if __name__ == "__main__":
    main()
