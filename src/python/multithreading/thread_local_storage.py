"""
Thread Local Storage (TLS) Example

This script demonstrates the use of threading.local() to create thread-specific
data storage. Each thread has its own independent copy of the data.

Key Concepts:
- threading.local() creates an object with thread-specific attributes
- Each thread sees only its own data, even with the same attribute names
- No synchronization needed - each thread has its own isolated storage

Use Cases:
- Database connections per thread
- User session data in web applications
- Request context in server applications
- Thread-specific configuration or state

Alternative Approaches:
- Global dictionary with thread IDs as keys (requires manual locking)
- Pass context explicitly through function arguments
- Use contextvars (Python 3.7+) for async-compatible context
"""

import threading

# Create a thread-local data object
thread_local_data = threading.local()


def worker(thread_id):
    """
    Worker function that demonstrates thread-local storage.

    Each thread:
    1. Stores its ID in thread-local storage
    2. Performs work that modifies the thread-local data
    3. Reads and displays its thread-local data
    """
    # Store thread-local data
    thread_local_data.value = thread_id

    # Simulate some work that modifies thread-local data
    some_work()

    # Read thread-local data (each thread sees only its own value)
    print(f"Thread {thread_id} has value: {thread_local_data.value}")


def some_work():
    """Modify the thread-local data."""
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
