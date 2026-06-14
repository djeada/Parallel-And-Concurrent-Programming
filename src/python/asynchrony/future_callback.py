"""
Future Callback Example

This script demonstrates how to use callbacks with asyncio.Future objects.
Callbacks are functions that are automatically called when a Future completes,
allowing for event-driven programming patterns.

Key Concepts:
- Future.add_done_callback(): Registers a function to be called when Future completes
- Callbacks receive the Future object as their argument
- Use Future.result() inside the callback to get the actual result
- asyncio.create_task(): Schedules a coroutine and returns a Task

Use Cases:
- Decoupling result handling from result computation
- Event-driven architectures
- Logging or notification when operations complete
- Chaining operations without explicit await

Note: While callbacks are powerful, async/await syntax is often cleaner.
Use callbacks when you need to decouple concerns or integrate with callback-based APIs.
Always handle exceptions inside callbacks because result() re-raises task failures.
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
    try:
        print(f"Square callback called with result: {future.result()}")
    except Exception as exc:
        print(f"Square callback saw an error: {exc}")


async def main():
    """Demonstrate Future callbacks with async tasks."""
    start = time.perf_counter()

    tasks = [asyncio.create_task(slow_square(x)) for x in (3, 4)]
    for task in tasks:
        task.add_done_callback(square_callback)

    await asyncio.gather(*tasks)

    end = time.perf_counter()
    print(f"Asynchronous approach time: {end - start:.2f} seconds")


if __name__ == "__main__":
    asyncio.run(main())
