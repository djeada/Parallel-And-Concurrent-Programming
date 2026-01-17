"""
Threaded Merge Sort Example

This script demonstrates parallelizing the merge sort algorithm using threads.
It compares the performance of single-threaded vs multi-threaded implementations.

Key Concepts:
- Divide-and-conquer algorithms can benefit from parallelism
- ThreadPoolExecutor simplifies thread management
- Limiting recursion depth prevents thread explosion
- For CPU-bound tasks, threading may not help due to Python's GIL

Implementation Details:
- max_depth limits how deep we create new threads (prevents overhead)
- ThreadPoolExecutor manages thread lifecycle automatically
- Beyond max_depth, we fall back to regular recursion

Note: Due to Python's GIL, CPU-bound parallel merge sort may not be faster
with threads. For true parallelism with CPU-bound tasks, use multiprocessing.
This example still demonstrates the pattern for educational purposes.
"""

import random
import time
from concurrent.futures import ThreadPoolExecutor


def merge(left, right):
    """Merge two sorted lists into one sorted list."""
    result = []
    left_index = 0
    right_index = 0

    while left_index < len(left) and right_index < len(right):
        if left[left_index] < right[right_index]:
            result.append(left[left_index])
            left_index += 1
        else:
            result.append(right[right_index])
            right_index += 1

    # Append remaining elements
    result.extend(left[left_index:])
    result.extend(right[right_index:])

    return result


def merge_sort(arr):
    """Standard single-threaded merge sort."""
    if len(arr) <= 1:
        return arr

    mid = len(arr) // 2
    left = merge_sort(arr[:mid])
    right = merge_sort(arr[mid:])

    return merge(left, right)


def threaded_merge_sort(arr, depth=0, max_depth=2):
    """
    Multi-threaded merge sort with configurable parallelism depth.

    Args:
        arr: List to sort
        depth: Current recursion depth (for limiting thread creation)
        max_depth: Maximum depth to create new threads
    """
    if len(arr) <= 1:
        return arr

    mid = len(arr) // 2
    left = arr[:mid]
    right = arr[mid:]

    if depth < max_depth:
        # Parallelize at shallow depths
        with ThreadPoolExecutor(max_workers=2) as executor:
            left_future = executor.submit(
                threaded_merge_sort, left, depth + 1, max_depth
            )
            right_future = executor.submit(
                threaded_merge_sort, right, depth + 1, max_depth
            )
            left = left_future.result()
            right = right_future.result()
    else:
        # Fall back to sequential sorting at deeper levels
        left = threaded_merge_sort(left, depth + 1, max_depth)
        right = threaded_merge_sort(right, depth + 1, max_depth)

    return merge(left, right)


def main():
    arr = [random.randint(1, 10000) for _ in range(10000)]

    # Benchmark non-threaded version
    start_time = time.time()
    sorted_arr = merge_sort(arr.copy())
    end_time = time.time()
    print(f"Non-threaded merge sort time: {end_time - start_time:.5f} seconds")

    # Benchmark threaded version
    start_time = time.time()
    sorted_arr_threaded = threaded_merge_sort(arr.copy())
    end_time = time.time()
    print(f"Threaded merge sort time: {end_time - start_time:.5f} seconds")

    # Verify both produce the same result
    assert sorted_arr == sorted_arr_threaded, "Results don't match!"
    print("Both methods produced identical sorted results.")


if __name__ == "__main__":
    main()
