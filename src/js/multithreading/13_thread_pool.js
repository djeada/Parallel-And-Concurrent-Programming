"""
This script demonstrates the use of concurrent.futures.ThreadPoolExecutor to create
and manage a thread pool, efficiently executing multiple tasks using a fixed number
of worker threads.
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

    with concurrent.futures.ThreadPoolExecutor(max_workers=num_workers) as executor:
        # Submit tasks to the executor and store the resulting Future objects
        futures = [executor.submit(worker, i) for i in range(num_tasks)]

        # Collect the results as the tasks complete
        for future in concurrent.futures.as_completed(futures):
            task_id, result = future.result()
            print(f"Task {task_id} result collected: {result}")

if __name__ == "__main__":
    main()

