"""
This program demonstrates the use of a semaphore with multiprocessing in Python.
A semaphore is a synchronization primitive that can be used to control access to
a shared resource by multiple processes. Semaphores are often used to limit the
number of processes that can access a resource at the same time. This can be
useful for managing access to resources with limited capacity, such as
network connections or shared memory.

In this example, we create a pool of worker processes and use a semaphore to
limit the number of processes that can access a shared resource (in this case,
printing to the console) at the same time. The worker processes acquire the
semaphore before accessing the shared resource and release it when they are
finished. This ensures that the maximum number of concurrent accesses to the
shared resource is limited by the semaphore's initial value.
"""

import multiprocessing
import time
import random

# The shared resource is printing to the console
def shared_resource(process_id):
    print(f"Process {process_id} is using the shared resource")
    time.sleep(random.uniform(0.5, 1))
    print(f"Process {process_id} is done using the shared resource")


def worker(semaphore, process_id):
    print(f"Process {process_id} is waiting for the semaphore")
    with semaphore:
        print(f"Process {process_id} acquired the semaphore")
        shared_resource(process_id)
        print(f"Process {process_id} released the semaphore")


def main():
    num_processes = 10
    max_concurrent = 3

    # Create a semaphore with an initial value of max_concurrent
    semaphore = multiprocessing.Semaphore(max_concurrent)

    # Create a pool of worker processes
    with multiprocessing.Pool(processes=num_processes) as pool:
        # Execute the worker function with the semaphore and process IDs
        pool.starmap(worker, [(semaphore, i) for i in range(num_processes)])


if __name__ == "__main__":
    main()
