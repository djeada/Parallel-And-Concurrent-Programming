"""
This example demonstrates how to use asyncio.Semaphore to limit the number of concurrent tasks running in an async environment. It simulates a scenario where multiple worker tasks need to access a shared resource or perform an operation with limited capacity. The asyncio.Semaphore is used to ensure that only a specified number of workers can execute the resource-limited operation concurrently.
"""

import asyncio
import random

async def resource_limited_operation(id):
    print(f"Worker {id} is entering the resource-limited operation")
    await asyncio.sleep(random.uniform(0.5, 2))
    print(f"Worker {id} is leaving the resource-limited operation")

async def worker(sem, id):
    for _ in range(3):
        async with sem:
            await resource_limited_operation(id)
        await asyncio.sleep(random.uniform(0.5, 1))
    print(f"End of worker {id} function")

async def task_generator():
    sem = asyncio.Semaphore(value=2)  # Limit concurrency to 2
    n_workers = 5
    await asyncio.gather(*[worker(sem, i) for i in range(n_workers)])

async def main():
    await task_generator()

if __name__ == "__main__":
    asyncio.run(main())

print("The End")

