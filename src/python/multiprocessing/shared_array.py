"""
Shared Array Example

This script demonstrates sharing an array between processes using
multiprocessing.Array. This provides fast shared memory access for
sequences of primitive values.

Key Concepts:
- Array creates a ctypes array in shared memory
- Type codes: 'i' (int), 'd' (double), 'f' (float), etc.
- Access elements using index notation or iteration
- Use a Lock for process-safe modification
- Protect a whole logical update, not each element separately

Note:
This example runs two processes that modify the same array for a fixed
number of iterations. The lock protects each full-array update so another
process cannot observe a half-updated array.

For numpy arrays, consider multiprocessing.shared_memory (Python 3.8+)
or numpy.memmap for more efficient shared array operations.
"""

import multiprocessing
import time


def increment_array(shared_array, lock, num_iterations=5):
    """Increment all elements in the shared array."""
    for _ in range(num_iterations):
        with lock:
            for i in range(len(shared_array)):
                shared_array[i] += 1
            print(f"Array incremented: {list(shared_array)}")
        time.sleep(1)


def decrement_array(shared_array, lock, num_iterations=5):
    """Decrement all elements in the shared array."""
    for _ in range(num_iterations):
        with lock:
            for i in range(len(shared_array)):
                shared_array[i] -= 1
            print(f"Array decremented: {list(shared_array)}")
        time.sleep(1)


def main():
    # Create shared array of integers initialized to [0, 0, 0]
    shared_array = multiprocessing.Array("i", [0, 0, 0], lock=False)
    lock = multiprocessing.Lock()

    process1 = multiprocessing.Process(
        target=increment_array, args=(shared_array, lock, 5)
    )
    process2 = multiprocessing.Process(
        target=decrement_array, args=(shared_array, lock, 5)
    )

    process1.start()
    process2.start()

    process1.join()
    process2.join()

    print(f"Final array state: {list(shared_array)}")


if __name__ == "__main__":
    main()
