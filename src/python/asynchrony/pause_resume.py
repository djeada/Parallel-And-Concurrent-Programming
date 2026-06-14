"""
Pause and Resume Example

This script demonstrates how to implement pause/resume functionality for
async operations using asyncio.Event. This pattern is useful for building
responsive applications that can be controlled by user input.

Key Concepts:
- asyncio.Event: A simple synchronization primitive for coroutines
- Event.set()/clear(): Toggle the event state
- Event.is_set(): Check the current state
- asyncio.to_thread(): Run blocking I/O like input() without blocking the event loop

Use Cases:
- Pause/resume functionality in async applications
- User-controlled interruption of background tasks
- Implementing stop/start buttons in async GUIs
- Graceful pause for maintenance in servers

Note: This script runs indefinitely. Press Ctrl+C to exit.
Each ENTER press toggles between pause and resume states.
"""

import asyncio

resume_event = asyncio.Event()


async def long_function():
    """
    Long-running function that can be paused and resumed.

    When resume_event is cleared, the function pauses at the next checkpoint.
    """
    i = 0
    resume_event.set()
    while True:
        await resume_event.wait()
        print(f"Executing step {i}")
        i += 1
        await asyncio.sleep(1)


async def button_handler():
    """
    Handle user input to toggle pause/resume.

    Uses asyncio.to_thread to avoid blocking the event loop with input().
    """
    while True:
        await asyncio.to_thread(input, "Press ENTER to toggle pause/resume: ")
        if resume_event.is_set():
            print("Paused...")
            resume_event.clear()
        else:
            print("Resumed!")
            resume_event.set()


async def main():
    """Run the pausable function and button handler concurrently."""
    task1 = asyncio.create_task(long_function())
    task2 = asyncio.create_task(button_handler())

    try:
        await asyncio.gather(task1, task2)
    finally:
        task1.cancel()
        task2.cancel()


if __name__ == "__main__":
    asyncio.run(main())
