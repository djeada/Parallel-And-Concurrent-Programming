"""
Race Condition Example

This script demonstrates a race condition - a common bug in concurrent programming
where multiple threads access and modify shared data without proper synchronization.

Key Concepts:
- Race conditions occur when threads read-modify-write shared data non-atomically
- The final result depends on unpredictable thread scheduling
- Running this script multiple times may produce different results

The Problem:
1. Thread A reads COUNTER = 10
2. Thread B reads COUNTER = 10 (before A writes)
3. Thread A writes COUNTER = 20 (10 * 2)
4. Thread B writes COUNTER = 30 (10 * 3) - Thread A's update is lost!

Run this script multiple times to observe different final values of COUNTER.
See mutex.py and semaphore.py for solutions to this problem.
"""

import time
from threading import Thread

COUNTER = 1


def multiply_counter(multiplier):
    """Multiply the global COUNTER - demonstrates unsafe concurrent access."""
    global COUNTER
    for _ in range(10):
        time.sleep(0.1)
        # This read-modify-write sequence is NOT atomic
        local_counter = COUNTER  # Read
        local_counter *= multiplier  # Modify
        print(f"The COUNTER gets multiplied by {multiplier}")
        COUNTER = local_counter  # Write - other threads may have changed COUNTER!


def main():
    threads = []
    for i in range(10):
        threads.append(Thread(target=multiply_counter, args=(i + 1,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print(f"The final value of COUNTER is: {COUNTER}")


if __name__ == "__main__":
    main()
