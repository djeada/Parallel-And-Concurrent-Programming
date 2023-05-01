"""
This script demonstrates the use of a barrier with multiprocessing in Python.
A barrier is a synchronization primitive that allows multiple processes to
wait for each other at a certain point in their execution. When all processes
reach the barrier, they are allowed to continue executing.

In this example, we create a number of worker processes that perform some work,
and then wait at a barrier. Once all worker processes have reached the barrier,
they are allowed to continue and finish their execution. We use the
multiprocessing.Barrier class to implement the barrier.

Note that barriers can be useful in cases where the processes need to work in
lockstep or to ensure that certain resources are not accessed before all
processes are ready.
"""

import multiprocessing
import time
import random


def worker(barrier, worker_id):
    # Simulate some work
    sleep_time = random.uniform(1, 3)
    time.sleep(sleep_time)
    print(f"Worker {worker_id} finished initial work and is waiting at the barrier...")

    # Wait at the barrier
    barrier.wait()

    print(f"Worker {worker_id} passed the barrier and is continuing execution...")


def main():
    num_workers = 5

    # Create a barrier with num_workers as the number of participants
    barrier = multiprocessing.Barrier(num_workers)

    # Start worker processes
    processes = [
        multiprocessing.Process(target=worker, args=(barrier, i))
        for i in range(num_workers)
    ]

    for p in processes:
        p.start()

    for p in processes:
        p.join()


if __name__ == "__main__":
    main()
