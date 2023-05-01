"""
This script demonstrates the use of a shared value between multiple processes in Python
using the `Value` class from the `multiprocessing` module. The `Value` class provides a simple
and thread-safe way to share a single value between processes.

In this example, we create two processes: an incrementer and a decrementer. The incrementer increases
the shared value by 1, while the decrementer decreases the shared value by 1. We use a lock to ensure
that the operations on the shared value are synchronized and prevent race conditions.
"""

import multiprocessing
import time


def incrementer(shared_value, lock):
    for _ in range(5):
        with lock:
            shared_value.value += 1
            print(f"Incrementer: Increased shared value to {shared_value.value}")
        time.sleep(0.1)


def decrementer(shared_value, lock):
    for _ in range(5):
        with lock:
            shared_value.value -= 1
            print(f"Decrementer: Decreased shared value to {shared_value.value}")
        time.sleep(0.1)


def main():
    shared_value = multiprocessing.Value(
        "i", 0
    )  # Initialize an integer shared value with the initial value of 0
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
