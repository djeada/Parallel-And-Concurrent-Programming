"""
This script demonstrates the use of the multiprocessing module in Python to create
and manage a process pool. The process pool efficiently executes multiple tasks using
a fixed number of worker processes, allowing for parallelism and taking advantage of
multiple CPU cores. The multiprocessing module is used when the tasks are CPU-bound,
which can benefit from parallel execution on multiple cores. This is in contrast to
thread-based parallelism, which is generally more suited for I/O-bound tasks.

In this example, we use the ProcessPoolExecutor class from the concurrent.futures module
to manage the process pool. We submit tasks to the pool and collect the results as the tasks
are completed.
"""

import concurrent.futures
import random
import time


def worker(task_id):
    print(f"Task {task_id} is starting...")
    time.sleep(random.uniform(1, 3))  # Simulate some work
    result = task_id * 2
    print(f"Task {task_id} is finished. Result: {result}")
    return task_id, result


def main():
    num_tasks = 10
    num_workers = 3

    with concurrent.futures.ProcessPoolExecutor(max_workers=num_workers) as executor:
        # Submit tasks to the executor and store the resulting Future objects
        futures = [executor.submit(worker, i) for i in range(num_tasks)]

        # Collect the results as the tasks complete
        for future in concurrent.futures.as_completed(futures):
            task_id, result = future.result()
            print(f"Task {task_id} result collected: {result}")


if __name__ == "__main__":
    main()
