"""
Async Queue for Data Sharing

This script demonstrates using asyncio.Queue for safe data sharing and
communication between async coroutines. Unlike threading queues, asyncio.Queue
is designed specifically for async/await patterns.

Key Concepts:
- asyncio.Queue: Thread-safe, async-aware FIFO queue
- queue.put(): Add items (awaitable)
- queue.get(): Remove and return items (awaitable)
- Use None as sentinel value to signal termination

Use Cases:
- Producer-consumer patterns in async code
- Buffering between fast producers and slow consumers
- Task distribution in async workers
- Stream processing pipelines

Difference from threading.Queue:
- asyncio.Queue uses await instead of blocking
- Designed for single-threaded async code
- No need for locks within async context
"""

import asyncio
import random


async def producer(queue):
    """Produce items and put them into the queue."""
    for i in range(10):
        await asyncio.sleep(random.uniform(0.5, 1))
        print(f"Producer: Produced item {i}")
        await queue.put(i)

    # Signal consumer to stop
    await queue.put(None)


async def consumer(queue):
    """Consume items from the queue until receiving None."""
    while True:
        item = await queue.get()
        if item is None:
            break
        print(f"Consumer: Consumed item {item}")
        await asyncio.sleep(random.uniform(0.5, 1))


async def main():
    """Run producer and consumer concurrently."""
    queue = asyncio.Queue()

    producer_task = asyncio.create_task(producer(queue))
    consumer_task = asyncio.create_task(consumer(queue))

    await asyncio.gather(producer_task, consumer_task)


if __name__ == "__main__":
    asyncio.run(main())
