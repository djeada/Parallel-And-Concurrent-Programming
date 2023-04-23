"""
This script demonstrates the use of threading.local() to implement
Thread Local Storage (TLS) in Python. Thread Local Storage (TLS) allows each
thread to have its own private data that is independent of data in other threads.

In this script, each worker thread sets and reads its own thread-local data.
The main thread creates multiple worker threads, and each worker thread stores
its thread_id in the thread-local data. Then, the worker performs a computation
(simulated by some_work()) that modifies the thread-local data. Finally, each
worker thread reads and prints its thread-local data.

There are other ways to implement TLS, such as using a global dictionary with
thread identifiers as keys, but using threading.local() provides a more
convenient and efficient way to manage thread-local data.

The threading.local() class used in this script creates an instance with
attributes that are unique for each thread. When a thread accesses the
thread-local data, it sees only its own data and is unaware of the data in other
threads.
"""

import threading

# Create a thread-local data object
thread_local_data = threading.local()


def worker(thread_id):
    # Store thread-local data
    thread_local_data.value = thread_id

    # Simulate some work
    some_work()

    # Read thread-local data
    print(f"Thread {thread_id} has value: {thread_local_data.value}")


def some_work():
    thread_local_data.value *= 2


def main():
    num_threads = 5
    threads = [threading.Thread(target=worker, args=(i,)) for i in range(num_threads)]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()


if __name__ == "__main__":
    main()
