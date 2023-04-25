"""
This program demonstrates the use of a Manager object from the multiprocessing
module in Python for sharing data between processes. The Manager object provides
a high-level way to share data between processes, as it creates a server process
that manages shared objects for the other processes.

In this example, we create a Manager object that manages a shared list, which
is used by multiple processes. Each process appends a unique value to the shared
list. After all the processes have completed, the main process prints the contents
of the shared list.

The multiprocessing.Manager class provides an easy-to-use interface for sharing
data between processes, without the need for explicit synchronization. This approach
is suitable for cases where the shared data structures are more complex or when
using shared memory is not feasible.
"""

import multiprocessing


def add_to_list(shared_list, value):
    shared_list.append(value)
    print(f"Process {value} added {value} to the shared list")


def main():
    manager = multiprocessing.Manager()
    shared_list = manager.list()

    num_processes = 5
    processes = []

    for i in range(num_processes):
        process = multiprocessing.Process(target=add_to_list, args=(shared_list, i))
        processes.append(process)
        process.start()

    for process in processes:
        process.join()

    print("Shared list after all processes have finished:")
    print(shared_list)


if __name__ == "__main__":
    main()
