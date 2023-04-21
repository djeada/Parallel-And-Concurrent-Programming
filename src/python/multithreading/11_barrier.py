"""
This script demonstrates the use of the threading.Barrier class in Python
to synchronize multiple threads at a specific point in their execution. The worker
threads perform some work, wait at the barrier, and then continue after all
threads have reached the barrier.

A barrier is a synchronization primitive that can be used when you need a group of
threads to wait for each other before proceeding. The barrier can be useful in various
scenarios, such as:

- Performing parallel tasks that depend on the completion of each other.
- Ensuring that resources are loaded and ready before starting a new phase of processing.
- Waiting for a group of threads to complete a specific task before proceeding.

An alternative solution to using a barrier is using condition variables, semaphores,
or other synchronization primitives. However, the Barrier class provides a simple
and intuitive solution for cases where you want to synchronize a group of threads at
a specific point.

In this example, the worker function simulates some work by sleeping for a random
interval and then waits at the barrier. When all worker threads have reached the barrier,
they can proceed with their execution. The main function creates a barrier for a specified
number of threads, launches the worker threads, and waits for them to finish.
"""

import threading
import time
import random

def worker(barrier, thread_id):
    print(f"Thread {thread_id} is starting...")
    time.sleep(random.uniform(1, 3)) # Simulate some work
    print(f"Thread {thread_id} is waiting at the barrier...")

    barrier.wait()  # Wait for all threads to reach the barrier

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

if name == "main":
    main()
