"""
Future and create_task Example

This script demonstrates asyncio.create_task() for managing asynchronous
operations. Tasks are Future-like objects that represent running coroutines.

Key Concepts:
- asyncio.create_task(): Schedules a coroutine to run concurrently
- asyncio.gather(): Collects results from multiple awaitables

When to Use:
- create_task(): When you want to run coroutines concurrently
- Manual Future management is mainly for callback interop and low-level libraries

Pitfall:
- Do not create a Future just to hold a task result. A Task already is awaitable
  and stores its result or exception.
"""

import asyncio
import time


def slow_square_sync(x):
    """Synchronous version - blocks during computation."""
    print(f"Starting slow square computation for {x}")
    time.sleep(2)
    result = x * x
    print(f"Finished slow square computation for {x}")
    return result


async def slow_square_async(x):
    """Asynchronous version - yields during simulated computation."""
    print(f"Starting slow square computation for {x}")
    await asyncio.sleep(2)
    result = x * x
    print(f"Finished slow square computation for {x}")
    return result


def synchronous_execution():
    """Run computations synchronously (sequentially)."""
    start_time = time.time()

    result1 = slow_square_sync(3)
    result2 = slow_square_sync(4)

    elapsed_time = time.time() - start_time
    print(f"\nSynchronous execution took {elapsed_time} seconds.")
    print(f"Results: {result1}, {result2}")


async def asynchronous_execution():
    """Run computations asynchronously using create_task."""
    start_time = time.time()

    task1 = asyncio.create_task(slow_square_async(3))
    task2 = asyncio.create_task(slow_square_async(4))

    results = await asyncio.gather(task1, task2)

    elapsed_time = time.time() - start_time
    print(f"\nAsynchronous execution took {elapsed_time} seconds.")
    print(f"Results: {results}")


if __name__ == "__main__":
    synchronous_execution()
    asyncio.run(asynchronous_execution())
