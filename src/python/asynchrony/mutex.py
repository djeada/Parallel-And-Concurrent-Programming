import asyncio
import time


async def foo(lock, id):
    async with lock:
        for _ in range(5):
            print(f"Worker {id} is going to sleep")
            await asyncio.sleep(1)
    print(f"End of worker {id} function")


async def task_generator():
    lock = asyncio.Lock()
    n_workers = 3
    await asyncio.wait([foo(lock, i) for i in range(n_workers)])


lock = asyncio.Lock()
loop = asyncio.get_event_loop()
loop.run_until_complete(task_generator())
loop.close()

print("The End")
