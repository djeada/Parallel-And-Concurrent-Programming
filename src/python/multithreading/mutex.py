"""
Mutex (Lock) Example

This script demonstrates the use of a mutex (mutual exclusion lock) to protect
shared resources in a multi-threaded application. A mutex ensures that only
one thread at a time can execute a critical section of code.

Key Concepts:
- threading.Lock provides mutual exclusion
- Use 'with mutex:' for automatic acquire/release (context manager pattern)
- Prevents race conditions by serializing access to shared resources

Without the mutex, threads would interleave their prints unpredictably.
With the mutex, each thread completes its entire printing loop before
the next thread starts, providing deterministic output.

Try This:
Comment out 'with mutex:' and observe how the output becomes interleaved
as threads compete for the print statement.
"""

import time
from threading import Thread, Lock

mutex = Lock()


def print_repeatedly(thread_id):
    """Print thread ID 100 times, protected by a mutex."""
    with mutex:  # Comment out this line to see interleaved output
        for _ in range(100):
            print("Thread id:", thread_id)
            time.sleep(0.05)
    # The mutex is automatically released when exiting the 'with' block


def main():
    print("MAIN FUNCTION STARTS")

    threads = []
    for i in range(4):
        threads.append(Thread(target=print_repeatedly, args=(i,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print("THE END")


if __name__ == "__main__":
    main()
