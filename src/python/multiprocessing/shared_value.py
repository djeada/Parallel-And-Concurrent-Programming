"""
Shared Value Example

This script demonstrates sharing a single value between processes using
multiprocessing.Value. This provides a fast, low-level way to share
primitive data types across processes using shared memory.

Key Concepts:
- Value creates a ctypes object in shared memory
- Type codes: 'i' (int), 'd' (double), 'c' (char), etc.
- Access the value using .value attribute
- Use Lock for thread-safe access when multiple processes modify the value

When to Use:
- Sharing simple values (counters, flags, status indicators)
- When performance is critical (faster than Queue/Manager)
- When you need atomic read-modify-write operations

For complex data structures, consider using Manager instead.
"""

import multiprocessing
import time


def incrementer(shared_value, lock):
    """Increment the shared value 5 times."""
    for _ in range(5):
        with lock:
            shared_value.value += 1
            print(f"Incrementer: Increased shared value to {shared_value.value}")
        time.sleep(0.1)


def decrementer(shared_value, lock):
    """Decrement the shared value 5 times."""
    for _ in range(5):
        with lock:
            shared_value.value -= 1
            print(f"Decrementer: Decreased shared value to {shared_value.value}")
        time.sleep(0.1)


def main():
    # Initialize an integer shared value with value 0
    # 'i' = signed integer, see ctypes for other type codes
    shared_value = multiprocessing.Value("i", 0)
    lock = multiprocessing.Lock()

    incrementer_process = multiprocessing.Process(
        target=incrementer, args=(shared_value, lock)
    )
    decrementer_process = multiprocessing.Process(
        target=decrementer, args=(shared_value, lock)
    )

    incrementer_process.start()
    decrementer_process.start()

    incrementer_process.join()
    decrementer_process.join()

    print(f"Final shared value: {shared_value.value}")


if __name__ == "__main__":
    main()
