"""
This script demonstrates the use of threading to perform a merge sort on a list of integers.
The merge sort function is implemented both with and without threading, and the time taken
for both methods is compared.
"""

import random
import time
from concurrent.futures import ThreadPoolExecutor


def merge_sort(arr):
    if len(arr) <= 1:
        return arr

    mid = len(arr) // 2
    left = arr[:mid]
    right = arr[mid:]

    left = merge_sort(left)
    right = merge_sort(right)

    return merge(left, right)


def merge(left, right):
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

    if left_index < len(left):
        result.extend(left[left_index:])
    if right_index < len(right):
        result.extend(right[right_index:])

    return result


def threaded_merge_sort(arr, depth=0, max_depth=2):
    if len(arr) <= 1:
        return arr

    mid = len(arr) // 2
    left = arr[:mid]
    right = arr[mid:]

    if depth < max_depth:
        with ThreadPoolExecutor(max_workers=8) as executor:
            left_future = executor.submit(
                threaded_merge_sort, left, depth + 1, max_depth
            )
            right_future = executor.submit(
                threaded_merge_sort, right, depth + 1, max_depth
            )
            left = left_future.result()
            right = right_future.result()
    else:
        left = threaded_merge_sort(left, depth + 1, max_depth)
        right = threaded_merge_sort(right, depth + 1, max_depth)

    return merge(left, right)


if __name__ == "__main__":
    arr = [random.randint(1, 10000) for _ in range(10000)]

    start_time = time.time()
    sorted_arr = merge_sort(arr)
    end_time = time.time()
    print(f"Non-threaded merge sort time: {end_time - start_time:.5f} seconds")

    start_time = time.time()
    sorted_arr_threaded = threaded_merge_sort(arr)
    end_time = time.time()
    print(f"Threaded merge sort time: {end_time - start_time:.5f} seconds")
