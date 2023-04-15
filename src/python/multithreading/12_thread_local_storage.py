"""
This script demonstrates the use of threading.local() to implement
Thread Local Storage (TLS) in Python. Each worker thread sets and reads
thread-local data, which is independent of data in other threads.
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
