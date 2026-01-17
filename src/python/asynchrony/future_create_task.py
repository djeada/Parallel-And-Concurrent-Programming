"""
Future and create_task Example

This script demonstrates asyncio.Future and asyncio.create_task() for managing
asynchronous operations. Futures represent the eventual result of an async
operation, while create_task() schedules a coroutine for execution.

Key Concepts:
- asyncio.Future: A low-level awaitable representing an eventual result
- asyncio.create_task(): Schedules a coroutine to run concurrently
- Future.set_result(): Manually sets the result of a Future
- asyncio.gather(): Collects results from multiple awaitables

When to Use:
- Futures: When you need fine-grained control over async result handling
- create_task(): When you want to run coroutines concurrently
- For most cases, create_task() is preferred over manual Future management

Note: In modern asyncio, create_task() is usually sufficient. Explicit Future
handling is mainly needed for advanced patterns or interop with callbacks.
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
    """Run computations asynchronously using create_task and Futures."""
    start_time = time.time()

    # Create tasks to run concurrently
    task1 = asyncio.create_task(slow_square_async(3))
    task2 = asyncio.create_task(slow_square_async(4))

    # Create Futures to store results
    future1 = asyncio.Future()
    future2 = asyncio.Future()

    # Await tasks and set Future results
    future1.set_result(await task1)
    future2.set_result(await task2)

    # Gather results from Futures
    results = await asyncio.gather(future1, future2)

    elapsed_time = time.time() - start_time
    print(f"\nAsynchronous execution took {elapsed_time} seconds.")
    print(f"Results: {results}")


if __name__ == "__main__":
    synchronous_execution()
    asyncio.run(asynchronous_execution())
