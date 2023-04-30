"""
This script demonstrates the use of asyncio.Queue for data sharing and communication
between async coroutines in Python. An asyncio.Queue is a simple way to send and receive
messages between coroutines in an asynchronous manner, allowing them to communicate
and synchronize their operations without the need for locks or other synchronization
primitives. In this example, we simulate a producer and consumer scenario where one
coroutine produces data and the other consumes it using an asyncio.Queue.
"""

import asyncio
import random


async def producer(queue):
    for i in range(10):
        await asyncio.sleep(random.uniform(0.5, 1))
        print(f"Producer: Produced item {i}")
        await queue.put(i)
    await queue.put(None)  # Signal the consumer to exit by sending None


async def consumer(queue):
    while True:
        item = await queue.get()
        if item is None:
            break
        print(f"Consumer: Consumed item {item}")
        await asyncio.sleep(random.uniform(0.5, 1))


async def main():
    queue = asyncio.Queue()
    producer_task = asyncio.create_task(producer(queue))
    consumer_task = asyncio.create_task(consumer(queue))
    await asyncio.gather(producer_task, consumer_task)


if __name__ == "__main__":
    asyncio.run(main())
