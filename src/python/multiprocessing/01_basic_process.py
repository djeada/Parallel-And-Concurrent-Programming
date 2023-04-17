"""
This script demonstrates the use of Python's multiprocessing module to create a new process.
A new process is created and a function 'foo' is executed within it. Information about the process
ID and the worker thread ID is printed both for the main function and the worker function.
"""

import os
import multiprocessing
import threading


def foo():
    print("Worker function")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.currentThread().ident}")
    print(f"Worker thread name: {threading.currentThread().getName()}")


if __name__ == "__main__":

    print("Main function")
    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.currentThread().ident}")
    print(f"Main thread name: {threading.currentThread().getName()}")

    process = multiprocessing.Process(target=foo)
    process.start()
    process.join()

    print("Main function finished")
