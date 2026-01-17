"""
Single Worker Thread Example

This script demonstrates the fundamentals of threading in Python:
- Creating a single worker thread using threading.Thread
- Understanding the relationship between processes and threads
- Accessing thread identification (ID and name)

Key Concepts:
- All threads within a process share the same process ID (PID)
- Each thread has a unique thread ID (ident)
- Thread names can be customized for debugging and monitoring

When run, this script shows that the main thread and worker thread
share the same process ID but have different thread IDs.
"""

import os
import threading
from threading import Thread


def worker():
    """Worker function that runs in a separate thread."""
    print("Worker function")
    print(f"Worker function process id: {os.getpid()}")
    print(f"Worker thread id: {threading.current_thread().ident}")
    print(f"Worker thread name: {threading.current_thread().name}")


def main():
    print(f"Main function process id: {os.getpid()}")
    print(f"Main thread id: {threading.current_thread().ident}")
    print(f"Main thread name: {threading.current_thread().name}")

    # Create a thread with a custom name for easier debugging
    thread = Thread(name="Example_thread", target=worker)
    thread.start()

    # Wait for the worker thread to complete
    thread.join()


if __name__ == "__main__":
    main()
