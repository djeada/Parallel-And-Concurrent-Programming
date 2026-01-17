"""
Thread Pool Example

This script demonstrates the use of concurrent.futures.ThreadPoolExecutor to
efficiently manage a pool of worker threads for executing multiple tasks.

Key Concepts:
- ThreadPoolExecutor manages thread lifecycle automatically
- submit() returns a Future object representing the pending result
- as_completed() yields Futures as they complete (not in submission order)
- Context manager ensures proper cleanup of threads

Advantages of Thread Pools:
- Reuse threads instead of creating/destroying for each task
- Limit maximum concurrent threads to prevent resource exhaustion
- Automatic task queuing when all workers are busy
- Clean shutdown with proper resource cleanup

For CPU-bound tasks, consider ProcessPoolExecutor instead to bypass the GIL.
"""

import concurrent.futures
import random
import time


def worker(task_id):
    """
    Worker function that simulates a task with variable execution time.

    Args:
        task_id: Identifier for the task

    Returns:
        Tuple of (task_id, result)
    """
    print(f"Task {task_id} is starting...")
    time.sleep(random.uniform(1, 3))  # Simulate variable work time
    result = task_id * 2
    print(f"Task {task_id} is finished. Result: {result}")
    return task_id, result


def main():
    num_tasks = 10
    num_workers = 3

    with concurrent.futures.ThreadPoolExecutor(max_workers=num_workers) as executor:
        # Submit all tasks to the executor
        futures = [executor.submit(worker, i) for i in range(num_tasks)]

        # Process results as they complete (not in submission order)
        for future in concurrent.futures.as_completed(futures):
            task_id, result = future.result()
            print(f"Task {task_id} result collected: {result}")


if __name__ == "__main__":
    main()
