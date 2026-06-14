"""
Running CPU-Bound Functions with Asyncio

This script demonstrates how to run CPU-bound (blocking) functions within
an asyncio application without blocking the event loop. It uses
ProcessPoolExecutor to offload heavy computations to separate processes.

Key Concepts:
- CPU-bound tasks block the event loop if run directly
- ProcessPoolExecutor: Runs functions in separate processes
- loop.run_in_executor(): Bridges sync functions to async context
- asyncio.to_thread(): Simpler API for running blocking I/O in threads

When to Use:
- ProcessPoolExecutor: For CPU-bound tasks (computation, number crunching)
- ThreadPoolExecutor: For blocking I/O (file, network with sync libraries)
- asyncio.to_thread(): For simple blocking I/O cases (Python 3.9+)

Note: This example is intentionally finite so it can be run in tests or classrooms.
"""

import asyncio
import concurrent.futures


def blocking_function(task_id):
    """
    CPU-bound function that would block the event loop if run directly.

    Performs integer arithmetic to simulate heavy computation.
    """
    print(f"Running blocking function {task_id}")
    total = sum(i * i for i in range(10_000_000))
    print(f"Done running blocking function {task_id}")
    return task_id, total


async def heartbeat():
    """Show that the event loop stays responsive while processes do CPU work."""
    for i in range(5):
        print(f"Event loop heartbeat {i}")
        await asyncio.sleep(0.5)


async def run_blocking_functions(executor, callback):
    """Run multiple blocking functions in a process pool."""
    print("Start run_blocking_functions")

    loop = asyncio.get_running_loop()
    print("Waiting for executor tasks")
    blocking_tasks = [loop.run_in_executor(executor, callback, i) for i in range(6)]
    results = await asyncio.gather(*blocking_tasks)
    print(f"Results: {results}")
    print("End run_blocking_functions")


async def main():
    """Run CPU-bound tasks and user input concurrently."""
    with concurrent.futures.ProcessPoolExecutor(max_workers=3) as executor:
        await asyncio.gather(
            run_blocking_functions(executor, blocking_function),
            heartbeat(),
        )


if __name__ == "__main__":
    asyncio.run(main())
