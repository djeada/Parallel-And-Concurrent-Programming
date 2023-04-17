"""
This script demonstrates the use of threads in Python.
It shows how to create a worker thread, retrieve its ID, and print the process ID.

When the script is executed, it will display the process ID and thread ID for both the main function
and the worker function. It will also display the thread names, which can be useful for debugging
and monitoring purposes.
"""

import os
import threading
from threading import Thread


def foo():
    print("Worker function")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.currentThread().ident}")
    print(f"Worker thread name: {threading.currentThread().getName()}")


if __name__ == "__main__":

    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.currentThread().ident}")
    print(f"Main thread name: {threading.currentThread().getName()}")

    thread = Thread(name="Example_thread", target=foo)
    thread.start()

    thread.join()
