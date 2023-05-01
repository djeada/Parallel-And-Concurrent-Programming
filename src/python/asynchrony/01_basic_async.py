"""
This script demonstrates the difference between synchronous and asynchronous
approaches to handling I/O-bound tasks using Python. The synchronous version
runs the tasks sequentially, while the asynchronous version takes advantage of
asyncio to run the tasks concurrently. This allows for better resource utilization
and improved performance when dealing with I/O-bound tasks.

The script contains two functions, `sync_main()` and `async_main()`, which
demonstrate the synchronous and asynchronous approaches, respectively. The
execution time for each approach is measured and displayed to show the performance
difference.

To see the difference in execution, simply uncomment the appropriate function call
in the `if __name__ == "__main__":` block.
"""

import time
import asyncio


def sync_task(task_id, sleep_time):
    print(f"Task {task_id} started.")
    time.sleep(sleep_time)  # Simulate an I/O-bound task
    print(f"Task {task_id} finished.")


def sync_main():
    start_time = time.time()

    sync_task(1, 2)
    sync_task(2, 1)

    elapsed_time = time.time() - start_time
    print(f"Synchronous execution took {elapsed_time:.2f} seconds.")


async def async_task(task_id, sleep_time):
    print(f"Task {task_id} started.")
    await asyncio.sleep(sleep_time)  # Simulate an I/O-bound task
    print(f"Task {task_id} finished.")


async def async_main():
    start_time = time.time()

    task1 = async_task(1, 2)
    task2 = async_task(2, 1)

    # Run the tasks concurrently
    await asyncio.gather(task1, task2)

    elapsed_time = time.time() - start_time
    print(f"Asynchronous execution took {elapsed_time:.2f} seconds.")


if __name__ == "__main__":
    sync_main()
    asyncio.run(async_main())
