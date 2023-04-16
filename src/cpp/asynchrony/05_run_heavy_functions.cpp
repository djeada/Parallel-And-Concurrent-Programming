"""
This example demonstrates how to run CPU-bound tasks concurrently using asyncio in combination with a ProcessPoolExecutor. This is useful when you want to achieve concurrency with asyncio but have CPU-bound tasks that would otherwise block the async event loop. The example shows how to offload these tasks to separate processes managed by the ProcessPoolExecutor, allowing the event loop to continue processing other async tasks without being blocked.
"""

import asyncio
import concurrent.futures
import time
import numpy as np


def blocking_function(i):
    print(f"Running blocking function {i}")
    a = np.random.rand(1000, 1000)
    b = np.random.rand(1000, 1000)
    c = np.dot(a, b)
    print(f"Done running blocking function {i}")
    return i


async def run_blocking_functions(executor, callback):
    print("Start run_blocking_functions")

    loop = asyncio.get_event_loop()
    blocking_tasks = [loop.run_in_executor(executor, callback, i) for i in range(6)]
    print("Waiting for executor tasks")
    completed, _ = await asyncio.wait(blocking_tasks)
    results = [task.result() for task in completed]
    print(f"Results: {results}")
    print("End run_blocking_functions")


def main():
    print("START main")
    executor = concurrent.futures.ProcessPoolExecutor(max_workers=3)

    asyncio.run(run_blocking_functions(executor, blocking_function))
    print("STOP main")


if __name__ == "__main__":
    main()
