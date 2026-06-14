"""
Semaphore Example

This script demonstrates synchronization using a semaphore. Unlike a mutex
(which allows only one thread), a semaphore limits access to a resource to N
threads simultaneously.

Key Concepts:
- Lock is the usual tool for protecting one shared critical section
- Semaphore(N) is the usual tool for representing N identical resource slots
- Use a semaphore as a context manager so release() happens automatically

Good Practice:
- Prefer Lock over Semaphore(1) for ordinary mutual exclusion because it states
  the intent more clearly.
- Prefer Semaphore(N) when the resource really has capacity N, such as a small
  connection pool, a download limit, or a fixed number of hardware devices.
"""

import random
import time
from threading import Semaphore, Thread

MAX_CONNECTIONS = 3
connection_slots = Semaphore(MAX_CONNECTIONS)


def use_connection(worker_id):
    """Simulate work that needs one slot from a limited connection pool."""
    print(f"Worker {worker_id} is waiting for a connection slot")

    with connection_slots:
        print(f"Worker {worker_id} acquired a connection slot")
        time.sleep(random.uniform(0.5, 1.5))
        print(f"Worker {worker_id} released a connection slot")


def main():
    threads = [Thread(target=use_connection, args=(i + 1,)) for i in range(10)]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print("All workers completed.")


if __name__ == "__main__":
    main()
