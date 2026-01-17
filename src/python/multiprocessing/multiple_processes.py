"""
Multiple Processes Example

This script demonstrates creating and managing multiple processes that execute
different functions concurrently. Each process runs in its own memory space
with its own Python interpreter instance.

Key Concepts:
- Multiple processes run truly in parallel on multi-core systems
- Each process has isolated memory (no shared state by default)
- Process execution order is non-deterministic
- Use join() to wait for all processes to complete

Unlike threads, multiple processes bypass the GIL, enabling true parallelism
for CPU-bound tasks. However, inter-process communication requires explicit
mechanisms like Queue, Pipe, or shared memory.
"""

import os
import multiprocessing
import threading
import time


def task_foo():
    """First worker function with a 1-second execution time."""
    print("Function foo starts")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.current_thread().ident}")
    print(f"Worker thread name: {threading.current_thread().name}")
    time.sleep(1)
    print("Function foo finishes")


def task_bar():
    """Second worker function with a 0.5-second execution time."""
    print("Function bar starts")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.current_thread().ident}")
    print(f"Worker thread name: {threading.current_thread().name}")
    time.sleep(0.5)
    print("Function bar finishes")


def main():
    print("Main function")
    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.current_thread().ident}")
    print(f"Main thread name: {threading.current_thread().name}")

    # Create processes for different tasks
    process_foo = multiprocessing.Process(target=task_foo)
    process_bar = multiprocessing.Process(target=task_bar)

    # Start both processes
    process_foo.start()
    process_bar.start()

    # Wait for both processes to complete
    process_foo.join()
    process_bar.join()

    print("Main function finished")


if __name__ == "__main__":
    main()
