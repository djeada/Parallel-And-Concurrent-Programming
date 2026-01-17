"""
Async Producer-Consumer Pattern

This script demonstrates the producer-consumer pattern using asyncio. Multiple
producers create items that are consumed by multiple consumers through a shared
queue, all running concurrently within a single thread.

Key Concepts:
- asyncio.Queue: Async-safe queue for inter-coroutine communication
- queue.task_done(): Marks a consumed item as processed
- queue.join(): Waits until all items are processed
- None sentinel: Signals consumers to terminate

Pattern Benefits:
- Decouples production from consumption
- Handles different production/consumption rates
- Enables concurrent processing without threads
- Natural backpressure when queue is bounded

This example uses 3 producers and 2 consumers to show how async
handles many-to-many producer-consumer relationships efficiently.
"""

import asyncio
import random


async def producer(queue, producer_id):
    """Produce items and add them to the queue."""
    for i in range(5):
        item = f"Item {i} from producer {producer_id}"
        await queue.put(item)
        print(f"Producer {producer_id} produced {item}")
        await asyncio.sleep(random.uniform(0.5, 2))


async def consumer(queue, consumer_id):
    """Consume items from the queue until receiving None sentinel."""
    while True:
        item = await queue.get()
        if item is None:
            break
        print(f"Consumer {consumer_id} consumed {item}")
        await asyncio.sleep(random.uniform(0.5, 2))
        queue.task_done()


async def main():
    """Coordinate multiple producers and consumers."""
    queue = asyncio.Queue()

    # Create producers and consumers
    producers = [asyncio.create_task(producer(queue, i)) for i in range(3)]
    consumers = [asyncio.create_task(consumer(queue, i)) for i in range(2)]

    # Wait for all producers to finish
    await asyncio.gather(*producers)

    # Wait for queue to be fully processed
    await queue.join()

    # Signal consumers to stop
    for _ in consumers:
        await queue.put(None)

    # Wait for consumers to finish
    await asyncio.gather(*consumers)


if __name__ == "__main__":
    asyncio.run(main())
