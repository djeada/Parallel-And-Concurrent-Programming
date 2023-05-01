"""
This script demonstrates the use of async generators in Python. Async generators
are a combination of async functions and generators. They are defined using the
`async def` keyword and can use `await` to perform asynchronous operations
inside the generator. The `yield` keyword is used to produce values just like
a regular generator.

The example script consists of an async generator function and a main function
that iterates over the async generator using the `async for` construct. The
example demonstrates how async generators can be used to fetch data
asynchronously and produce values as they become available.
"""

import asyncio
import random


async def async_generator():
    for i in range(5):
        # Simulate an asynchronous operation.
        await asyncio.sleep(random.uniform(0.5, 1.5))
        yield i


async def main():
    async for value in async_generator():
        print(f"Received value: {value}")


if __name__ == "__main__":
    asyncio.run(main())
