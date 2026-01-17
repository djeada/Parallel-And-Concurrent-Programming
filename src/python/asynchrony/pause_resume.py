"""
Pause and Resume Example

This script demonstrates how to implement pause/resume functionality for
async operations using asyncio.Event. This pattern is useful for building
responsive applications that can be controlled by user input.

Key Concepts:
- asyncio.Event: A simple synchronization primitive for coroutines
- Event.set()/clear(): Toggle the event state
- Event.is_set(): Check the current state
- run_in_executor(): Run blocking I/O (like input()) without blocking event loop

Use Cases:
- Pause/resume functionality in async applications
- User-controlled interruption of background tasks
- Implementing stop/start buttons in async GUIs
- Graceful pause for maintenance in servers

Note: This script runs indefinitely. Press Ctrl+C to exit.
Each ENTER press toggles between pause and resume states.
"""

import asyncio
from concurrent.futures import ThreadPoolExecutor

executor = ThreadPoolExecutor()
pause_event = asyncio.Event()


async def long_function():
    """
    Long-running function that can be paused and resumed.

    When pause_event is set, the function waits until it's cleared.
    """
    i = 0
    while True:
        if pause_event.is_set():
            print("Paused...")
            while pause_event.is_set():
                await asyncio.sleep(0.1)
            print("Resumed!")
        print(f"Executing step {i}")
        i += 1
        await asyncio.sleep(1)


async def button_handler():
    """
    Handle user input to toggle pause/resume.

    Uses run_in_executor to avoid blocking the event loop with input().
    """
    loop = asyncio.get_event_loop()
    while True:
        await loop.run_in_executor(
            executor, input, "Press ENTER to toggle pause/resume: "
        )
        if pause_event.is_set():
            pause_event.clear()
        else:
            pause_event.set()


async def main():
    """Run the pausable function and button handler concurrently."""
    task1 = asyncio.create_task(long_function())
    task2 = asyncio.create_task(button_handler())

    await asyncio.gather(task1, task2)


if __name__ == "__main__":
    asyncio.run(main())
