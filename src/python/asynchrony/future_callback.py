"""
Future Callback Example

This script demonstrates how to use callbacks with asyncio.Future objects.
Callbacks are functions that are automatically called when a Future completes,
allowing for event-driven programming patterns.

Key Concepts:
- Future.add_done_callback(): Registers a function to be called when Future completes
- Callbacks receive the Future object as their argument
- Use Future.result() inside the callback to get the actual result
- asyncio.ensure_future(): Alternative to create_task() for awaitable objects

Use Cases:
- Decoupling result handling from result computation
- Event-driven architectures
- Logging or notification when operations complete
- Chaining operations without explicit await

Note: While callbacks are powerful, async/await syntax is often cleaner.
Use callbacks when you need to decouple concerns or integrate with callback-based APIs.
"""

import asyncio
import time


async def slow_square(x):
    """Compute square asynchronously."""
    print(f"Starting slow square computation for {x}")
    await asyncio.sleep(1)
    print(f"Finished slow square computation for {x}")
    return x * x


def square_callback(future):
    """Callback function invoked when Future completes."""
    print(f"Square callback called with result: {future.result()}")


async def main():
    """Demonstrate Future callbacks with async tasks."""
    start = time.perf_counter()

    # Create tasks (ensure_future is an alternative to create_task)
    task1 = asyncio.ensure_future(slow_square(3))
    task2 = asyncio.ensure_future(slow_square(4))

    # Create Futures with callbacks
    future1 = asyncio.Future()
    future2 = asyncio.Future()

    future1.add_done_callback(square_callback)
    future2.add_done_callback(square_callback)

    # Set Future results (triggers callbacks)
    future1.set_result(await task1)
    future2.set_result(await task2)

    end = time.perf_counter()
    print(f"Asynchronous approach time: {end - start:.2f} seconds")


if __name__ == "__main__":
    asyncio.run(main())
