"""
This script demonstrates the use of asyncio.Future when working with async
functions in Python. It shows how to create a future, set a result, and
retrieve the result from a future.

The script contains a synchronous function and an async function, both simulating
a slow computation. We first run the synchronous version and measure the time taken.
Then, we run the asynchronous version with futures and measure the time taken.

This example shows the differences between synchronous and asynchronous approaches
and the effective use of futures to handle results from async functions.
"""

import asyncio
import time


def slow_square_sync(x):
    print(f"Starting slow square computation for {x}")
    time.sleep(2)
    result = x * x
    print(f"Finished slow square computation for {x}")
    return result


async def slow_square_async(x):
    print(f"Starting slow square computation for {x}")
    await asyncio.sleep(2)
    result = x * x
    print(f"Finished slow square computation for {x}")
    return result


def synchronous_execution():
    start_time = time.time()

    result1 = slow_square_sync(3)
    result2 = slow_square_sync(4)

    elapsed_time = time.time() - start_time
    print(f"\nSynchronous execution took {elapsed_time} seconds.")
    print(f"Results: {result1}, {result2}")


async def asynchronous_execution():
    start_time = time.time()

    future1 = asyncio.Future()
    future2 = asyncio.Future()

    task1 = asyncio.create_task(slow_square_async(3))
    task2 = asyncio.create_task(slow_square_async(4))

    future1.set_result(await task1)
    future2.set_result(await task2)

    results = await asyncio.gather(future1, future2)

    elapsed_time = time.time() - start_time
    print(f"\nAsynchronous execution took {elapsed_time} seconds.")
    print(f"Results: {results}")


if __name__ == "__main__":
    synchronous_execution()
    asyncio.run(asynchronous_execution())
