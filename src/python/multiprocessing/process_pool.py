"""
Process Pool Example

This script demonstrates using ProcessPoolExecutor to efficiently manage
a pool of worker processes for parallel execution of CPU-bound tasks.

Key Concepts:
- ProcessPoolExecutor manages process lifecycle automatically
- Processes are reused across tasks (avoids spawn overhead)
- submit() returns a Future representing the pending result
- as_completed() yields Futures as tasks finish (not submission order)

When to Use:
- CPU-bound tasks (computation, data processing, image manipulation)
- Tasks that can be parallelized across multiple cores
- When you need true parallelism (bypassing Python's GIL)

For I/O-bound tasks, consider ThreadPoolExecutor instead, as it has
lower overhead and the GIL doesn't affect I/O operations.
"""

import concurrent.futures
import random
import time


def worker(task_id):
    """
    Worker function that simulates a CPU-bound task.

    Args:
        task_id: Identifier for the task

    Returns:
        Tuple of (task_id, result)
    """
    print(f"Task {task_id} is starting...")
    time.sleep(random.uniform(1, 3))  # Simulate CPU-bound work
    result = task_id * 2
    print(f"Task {task_id} is finished. Result: {result}")
    return task_id, result


def main():
    num_tasks = 10
    num_workers = 3

    with concurrent.futures.ProcessPoolExecutor(max_workers=num_workers) as executor:
        # Submit all tasks to the executor
        futures = [executor.submit(worker, i) for i in range(num_tasks)]

        # Process results as they complete (not in submission order)
        for future in concurrent.futures.as_completed(futures):
            task_id, result = future.result()
            print(f"Task {task_id} result collected: {result}")


if __name__ == "__main__":
    main()
