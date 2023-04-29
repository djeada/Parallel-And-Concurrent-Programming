"""
This script demonstrates the use of asyncio.Future and callbacks in asynchronous programming. It compares the
asynchronous approach to a synchronous approach for computing square of numbers using slow_square function.

In the asynchronous approach, we create two tasks for computing squares and then create two Future objects.
We add a callback function 'square_callback' to the Future objects that will be called when the future is done.

The main function awaits the results of the slow_square function and sets the result for each future, which in turn
triggers the callback functions. This demonstrates how Future objects can be used for more advanced control flow and
how they can help decouple parts of your code.
"""

import asyncio
import time

async def slow_square(x):
    print(f"Starting slow square computation for {x}")
    await asyncio.sleep(1)
    print(f"Finished slow square computation for {x}")
    return x * x

def square_callback(future):
    print(f"Square callback called with result: {future.result()}")

async def main():
    start = time.perf_counter()
    task1 = asyncio.ensure_future(slow_square(3))
    task2 = asyncio.ensure_future(slow_square(4))

    future1 = asyncio.Future()
    future2 = asyncio.Future()

    future1.add_done_callback(square_callback)
    future2.add_done_callback(square_callback)

    future1.set_result(await task1)
    future2.set_result(await task2)

    end = time.perf_counter()
    print(f"Asynchronous approach time: {end - start}")

asyncio.run(main())
