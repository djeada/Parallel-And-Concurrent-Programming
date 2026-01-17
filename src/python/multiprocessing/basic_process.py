"""
Basic Process Example

This script demonstrates the fundamentals of multiprocessing in Python:
- Creating a child process using multiprocessing.Process
- Understanding the relationship between parent and child processes
- Accessing process and thread identification

Key Concepts:
- Each process has its own memory space (unlike threads which share memory)
- Child processes have different PIDs than the parent
- Each process has a main thread with its own thread ID
- Use join() to wait for child processes to complete

Unlike threads, processes bypass Python's GIL (Global Interpreter Lock),
making multiprocessing ideal for CPU-bound tasks.
"""

import os
import multiprocessing
import threading


def worker():
    """Worker function that runs in a child process."""
    print("Worker function")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.current_thread().ident}")
    print(f"Worker thread name: {threading.current_thread().name}")


def main():
    print("Main function")
    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.current_thread().ident}")
    print(f"Main thread name: {threading.current_thread().name}")

    # Create and start a child process
    process = multiprocessing.Process(target=worker)
    process.start()

    # Wait for the child process to complete
    process.join()

    print("Main function finished")


if __name__ == "__main__":
    main()
