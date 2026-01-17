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

Note: This example requires numpy. Install with: pip install numpy
The script runs indefinitely accepting user input. Press Ctrl+C to exit.
"""

import asyncio
import concurrent.futures
import numpy as np


def blocking_function(task_id):
    """
    CPU-bound function that would block the event loop if run directly.

    Performs matrix multiplication to simulate heavy computation.
    """
    print(f"Running blocking function {task_id}")
    a = np.random.rand(3000, 3000)
    b = np.random.rand(3000, 3000)
    c = np.dot(a, b)
    print(f"Done running blocking function {task_id}")
    return task_id


async def user_input():
    """Handle user input without blocking the event loop."""
    while True:
        user_text = await asyncio.to_thread(input, "Enter some text: \n")
        print(f"User just input: {user_text}")


async def run_blocking_functions(executor, callback):
    """Run multiple blocking functions in a process pool."""
    print("Start run_blocking_functions")

    loop = asyncio.get_running_loop()
    blocking_tasks = [loop.run_in_executor(executor, callback, i) for i in range(10)]

    print("Waiting for executor tasks")
    completed, _ = await asyncio.wait(blocking_tasks)
    results = [task.result() for task in completed]
    print(f"Results: {results}")
    print("End run_blocking_functions")


async def main():
    """Run CPU-bound tasks and user input concurrently."""
    executor = concurrent.futures.ProcessPoolExecutor(max_workers=3)

    await asyncio.gather(
        run_blocking_functions(executor, blocking_function),
        user_input(),
    )


if __name__ == "__main__":
    asyncio.run(main())
