"""
Mutex (Lock) Example

This script demonstrates the use of a mutex (mutual exclusion lock) to protect
shared resources in a multi-threaded application. A mutex ensures that only
one thread at a time can execute a critical section of code.

Key Concepts:
- threading.Lock provides mutual exclusion
- Use 'with mutex:' for automatic acquire/release (context manager pattern)
- Prevents race conditions by serializing access to shared resources
- Keep the critical section as small as possible

Without the mutex, the read-modify-write update to completed_tasks would be
unsafe. The sleep happens outside the lock because slow work should not block
other threads from entering the critical section.

Try This:
Comment out 'with mutex:' and increase the number of threads/tasks to make
lost updates more likely.
"""

import time
from threading import Thread, Lock

mutex = Lock()
completed_tasks = 0


def complete_task(thread_id):
    """Do work outside the lock, then update shared state inside the lock."""
    global completed_tasks

    print(f"Thread {thread_id} starts work")
    time.sleep(0.5)

    with mutex:
        completed_tasks += 1
        print(f"Thread {thread_id} finished; completed_tasks={completed_tasks}")


def main():
    print("MAIN FUNCTION STARTS")

    threads = []
    for i in range(4):
        threads.append(Thread(target=complete_task, args=(i,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print(f"THE END: {completed_tasks} tasks completed")


if __name__ == "__main__":
    main()
