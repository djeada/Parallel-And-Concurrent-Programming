"""
This script demonstrates the use of Python's multiprocessing module to create multiple processes.
Two new processes are created, and each process executes a different function - 'foo' and 'bar'.
Information about the process ID and the worker thread ID is printed both for the main function
and the worker functions.
"""

import os
import multiprocessing
import threading
import time


def foo():
    print("Function foo starts")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.currentThread().ident}")
    print(f"Worker thread name: {threading.currentThread().getName()}")
    time.sleep(1)
    print("Function foo finishes")


def bar():
    print("Function bar starts")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.currentThread().ident}")
    print(f"Worker thread name: {threading.currentThread().getName()}")
    time.sleep(0.5)
    print("Function bar finishes")


if __name__ == "__main__":

    print("Main function")
    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.currentThread().ident}")
    print(f"Main thread name: {threading.currentThread().getName()}")

    process_foo = multiprocessing.Process(target=foo)
    process_bar = multiprocessing.Process(target=bar)

    process_foo.start()
    process_bar.start()

    process_foo.join()
    process_bar.join()

    print("Main function finished")
