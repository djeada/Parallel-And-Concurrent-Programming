import asyncio
import random

async def critical_section(id):
    print(f"Worker {id} is entering the critical section")
    await asyncio.sleep(random.uniform(0.5, 2))
    print(f"Worker {id} is leaving the critical section")

async def foo(lock, id):
    for _ in range(3):
        async with lock:
            await critical_section(id)
        await asyncio.sleep(random.uniform(0.5, 1))
    print(f"End of worker {id} function")

async def task_generator():
    lock = asyncio.Lock()
    n_workers = 3
    await asyncio.gather(*[foo(lock, i) for i in range(n_workers)])

async def main():
    await task_generator()

if __name__ == "__main__":
    asyncio.run(main())

print("The End")
