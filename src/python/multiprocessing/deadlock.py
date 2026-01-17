"""
Deadlock Example

This script demonstrates a deadlock situation using multiprocessing. A deadlock
occurs when two or more processes are waiting for each other to release resources,
creating a circular dependency that can never be resolved.

Key Concepts:
- Deadlock requires: mutual exclusion, hold and wait, no preemption, circular wait
- Process A holds lock_a and waits for lock_b
- Process B holds lock_b and waits for lock_a
- Neither can proceed → deadlock!

Prevention Strategies:
- Always acquire locks in the same order across all processes
- Use timeouts when acquiring locks
- Use a single lock instead of multiple when possible
- Design lock-free algorithms where feasible

WARNING: This script will hang indefinitely due to the deadlock.
Use Ctrl+C to terminate it.
"""

import time
import multiprocessing as mp


def process_a(lock_a, lock_b):
    """Process A acquires lock_a first, then tries to acquire lock_b."""
    with lock_a:
        print("Process A acquired lock_a")
        time.sleep(1)  # Ensures Process B acquires lock_b
        print("Process A trying to acquire lock_b")
        with lock_b:
            print("Process A acquired lock_b")  # Never reached


def process_b(lock_a, lock_b):
    """Process B acquires lock_b first, then tries to acquire lock_a."""
    with lock_b:
        print("Process B acquired lock_b")
        time.sleep(1)  # Ensures Process A acquires lock_a
        print("Process B trying to acquire lock_a")
        with lock_a:
            print("Process B acquired lock_a")  # Never reached


def main():
    lock_a = mp.Lock()
    lock_b = mp.Lock()

    p_a = mp.Process(target=process_a, args=(lock_a, lock_b))
    p_b = mp.Process(target=process_b, args=(lock_a, lock_b))

    p_a.start()
    p_b.start()

    # These joins will never complete due to deadlock
    p_a.join()
    p_b.join()

    print("Main process finished")  # Never reached


if __name__ == "__main__":
    main()
