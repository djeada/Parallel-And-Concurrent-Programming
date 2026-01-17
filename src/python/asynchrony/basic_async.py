"""
Basic Async Example

This script demonstrates the fundamental difference between synchronous and
asynchronous programming in Python. It shows how async tasks can run concurrently,
significantly reducing total execution time for I/O-bound operations.

Key Concepts:
- async/await keywords define coroutines
- asyncio.sleep() is the async equivalent of time.sleep()
- asyncio.gather() runs multiple coroutines concurrently
- asyncio.run() starts the event loop and runs the main coroutine

Performance Comparison:
- Synchronous: Tasks run sequentially (total time = sum of all task times)
- Asynchronous: Tasks run concurrently (total time ≈ max task time)

When to Use Async:
- I/O-bound operations (network requests, file I/O, database queries)
- When you need to handle many concurrent connections
- NOT for CPU-bound tasks (use multiprocessing instead)
"""

import time
import asyncio


def sync_task(task_id, sleep_time):
    """Synchronous task that blocks during sleep."""
    print(f"Task {task_id} started.")
    time.sleep(sleep_time)  # Blocks the entire program
    print(f"Task {task_id} finished.")


def sync_main():
    """Run tasks synchronously (sequentially)."""
    start_time = time.time()

    sync_task(1, 2)
    sync_task(2, 1)

    elapsed_time = time.time() - start_time
    print(f"Synchronous execution took {elapsed_time:.2f} seconds.")


async def async_task(task_id, sleep_time):
    """Asynchronous task that yields control during sleep."""
    print(f"Task {task_id} started.")
    await asyncio.sleep(sleep_time)  # Yields control to event loop
    print(f"Task {task_id} finished.")


async def async_main():
    """Run tasks asynchronously (concurrently)."""
    start_time = time.time()

    task1 = async_task(1, 2)
    task2 = async_task(2, 1)

    # Run both tasks concurrently
    await asyncio.gather(task1, task2)

    elapsed_time = time.time() - start_time
    print(f"Asynchronous execution took {elapsed_time:.2f} seconds.")


if __name__ == "__main__":
    sync_main()
    asyncio.run(async_main())
