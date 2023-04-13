import asyncio
from concurrent.futures import ThreadPoolExecutor
import time

executor = ThreadPoolExecutor()

pause_event = asyncio.Event()

async def long_function():
    i = 0
    while True:
        if pause_event.is_set():
            while pause_event.is_set():
                await asyncio.sleep(0.1)
        print(f"Executing step {i}")
        i += 1
        await asyncio.sleep(1)

async def button_handler():
    loop = asyncio.get_event_loop()
    while True:
        await loop.run_in_executor(executor, input, "Press ENTER to toggle pause/resume:")
        if pause_event.is_set():
            pause_event.clear()
        else:
            pause_event.set()

async def main():
    task1 = asyncio.create_task(long_function())
    task2 = asyncio.create_task(button_handler())

    await asyncio.gather(task1, task2)

if __name__ == "__main__":
    asyncio.run(main())
