"""
Multiple Worker Threads Example

This script demonstrates creating and managing multiple threads that execute
different functions concurrently. It illustrates:
- Creating multiple threads with different target functions
- Passing arguments to thread target functions
- Thread interleaving and concurrent execution

Key Concepts:
- Threads can run different functions with varying execution times
- Thread execution order is non-deterministic (depends on OS scheduling)
- Using join() ensures all threads complete before the main thread continues

Observe how the output interleaves as threads with different sleep times
execute concurrently.
"""

import time
from threading import Thread


def foo(thread_id):
    """A worker function with a 1-second execution time."""
    print(f"Function foo() starts, Thread id: {thread_id}")
    time.sleep(1)
    print(f"Function foo() finishes, Thread id: {thread_id}")


def bar(thread_id):
    """A worker function with a 0.1-second execution time."""
    print(f"Function bar() starts, Thread id: {thread_id}")
    time.sleep(0.1)
    print(f"Function bar() finishes, Thread id: {thread_id}")


def main():
    print("Main thread starts")

    threads = []

    # Create 5 threads running foo()
    for i in range(5):
        threads.append(Thread(target=foo, args=(i,)))

    # Create 5 threads running bar()
    for i in range(5):
        threads.append(Thread(target=bar, args=(i,)))

    # Start all threads
    for thread in threads:
        thread.start()

    # Wait for all threads to complete
    for thread in threads:
        thread.join()

    print("Main thread finishes")


if __name__ == "__main__":
    main()
