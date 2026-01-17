"""
Semaphore Example

This script demonstrates synchronization using a semaphore. Unlike a mutex
(which allows only one thread), a semaphore can limit access to a resource
to N threads simultaneously.

Key Concepts:
- Semaphore(1) behaves like a mutex (binary semaphore)
- Semaphore(N) allows up to N threads in the critical section
- Useful for limiting access to resources with fixed capacity (e.g., connection pools)

Comparison with race_condition.py:
- Without synchronization: final value is unpredictable
- With semaphore: operations are properly serialized, final value is deterministic

Note: The final value will always be the same across runs because the
semaphore ensures atomic access to the COUNTER variable.
"""

import time
from threading import Thread, Semaphore

COUNTER = 1
semaphore = Semaphore(1)  # Binary semaphore (equivalent to a Lock)


def multiply_counter(multiplier):
    """Multiply the global COUNTER with semaphore protection."""
    global COUNTER
    for _ in range(10):
        time.sleep(0.1)
        with semaphore:
            # This entire block is now atomic
            local_counter = COUNTER
            local_counter *= multiplier
            print(f"The COUNTER gets multiplied by {multiplier}")
            COUNTER = local_counter


def main():
    threads = []
    for i in range(3):
        threads.append(Thread(target=multiply_counter, args=(i + 1,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print(f"The final value of COUNTER is: {COUNTER}")


if __name__ == "__main__":
    main()
