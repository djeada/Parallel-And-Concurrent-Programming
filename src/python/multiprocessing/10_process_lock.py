"""
This script demonstrates the use of a mutex (Lock) with the multiprocessing module in Python.
A mutex (short for "mutual exclusion") is a synchronization primitive that is used to prevent
multiple processes or threads from simultaneously accessing a shared resource, such as a file or
a variable in memory. In this example, we use the multiprocessing.Lock class to protect access
to a shared counter variable, ensuring that only one process can increment the counter at a time.
This prevents race conditions and ensures that the final value of the counter is correct.
"""

import multiprocessing
import time


def worker(mutex, counter):
    with mutex:
        print(f"Process {multiprocessing.current_process().name} is starting...")
        time.sleep(1)  # Simulate some work
        counter.value += 1
        print(f"Process {multiprocessing.current_process().name} is finished. Counter: {counter.value}")


def main():
    num_processes = 4
    counter = multiprocessing.Value('i', 0)  # Shared counter variable
    mutex = multiprocessing.Lock()  # Mutex to protect access to the counter

    processes = [multiprocessing.Process(target=worker, args=(mutex, counter)) for _ in range(num_processes)]

    for process in processes:
        process.start()

    for process in processes:
        process.join()

    print(f"Final counter value: {counter.value}")


if __name__ == "__main__":
    main()
