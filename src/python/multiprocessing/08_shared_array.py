"""
This script demonstrates how to share an array between multiple processes using
shared memory in Python. The multiprocessing module provides a Value and Array
class for sharing data between processes. The array is shared by creating a shared
memory object and allowing the processes to access it directly.

The script creates two child processes that operate on the shared array. One child
process increments the elements of the array, and the other decrements them. The
parent process creates the shared array and starts the child processes. This example
demonstrates the concepts of inter-process communication and sharing data between
processes in Python using shared memory.
"""

import multiprocessing
import time

def increment_array(shared_array):
    while True:
        for i in range(len(shared_array)):
            shared_array[i] += 1
        print(f"Array incremented: {[x for x in shared_array]}")
        time.sleep(1)

def decrement_array(shared_array):
    while True:
        for i in range(len(shared_array)):
            shared_array[i] -= 1
        print(f"Array decremented: {[x for x in shared_array]}")
        time.sleep(1)

def main():
    shared_array = multiprocessing.Array('i', [0, 0, 0])

    process1 = multiprocessing.Process(target=increment_array, args=(shared_array,))
    process2 = multiprocessing.Process(target=decrement_array, args=(shared_array,))

    process1.start()
    process2.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        process1.terminate()
        process2.terminate()
        process1.join()
        process2.join()

if __name__ == "__main__":
    main()
