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

ANTI-PATTERN DEMO:
This script intentionally creates a deadlock, but the parent detects that the
children are stuck and terminates them. This keeps the example safe to run
while still demonstrating why inconsistent lock ordering is dangerous.
"""

import time
import multiprocessing as mp


def process_a(lock_a, lock_b):
    """Process A acquires lock_a first, then tries to acquire lock_b."""
    with lock_a:
        print("Process A acquired lock_a", flush=True)
        time.sleep(1)  # Ensures Process B acquires lock_b
        print("Process A trying to acquire lock_b", flush=True)
        with lock_b:
            print("Process A acquired lock_b", flush=True)  # Never reached


def process_b(lock_a, lock_b):
    """Process B acquires lock_b first, then tries to acquire lock_a."""
    with lock_b:
        print("Process B acquired lock_b", flush=True)
        time.sleep(1)  # Ensures Process A acquires lock_a
        print("Process B trying to acquire lock_a", flush=True)
        with lock_a:
            print("Process B acquired lock_a", flush=True)  # Never reached


def main():
    lock_a = mp.Lock()
    lock_b = mp.Lock()

    p_a = mp.Process(target=process_a, args=(lock_a, lock_b))
    p_b = mp.Process(target=process_b, args=(lock_a, lock_b))

    p_a.start()
    p_b.start()

    # These joins time out because the child processes are deadlocked.
    p_a.join(timeout=3)
    p_b.join(timeout=3)

    for process in (p_a, p_b):
        if process.is_alive():
            print(f"{process.name} is deadlocked; terminating it for cleanup.")
            process.terminate()
            process.join()

    print("Main process finished after cleaning up the deadlocked children.")


if __name__ == "__main__":
    main()
