"""
This script demonstrates the producer-consumer pattern using Python's asyncio library.
The producer creates items and puts them into a queue, while the consumer takes items from the queue and processes them.
This pattern is useful when you want to separate the creation and processing of items, allowing them to run concurrently.
"""

import asyncio
import random


async def producer(queue, producer_id):
    for i in range(5):
        item = f"Item {i} from producer {producer_id}"
        await queue.put(item)
        print(f"Producer {producer_id} produced {item}")
        await asyncio.sleep(random.uniform(0.5, 2))


async def consumer(queue, consumer_id):
    while True:
        item = await queue.get()
        if item is None:
            break
        print(f"Consumer {consumer_id} consumed {item}")
        await asyncio.sleep(random.uniform(0.5, 2))
        queue.task_done()


async def main():
    queue = asyncio.Queue()

    producers = [asyncio.create_task(producer(queue, i)) for i in range(3)]
    consumers = [asyncio.create_task(consumer(queue, i)) for i in range(2)]

    await asyncio.gather(*producers)
    await queue.join()

    for _ in consumers:
        await queue.put(None)

    await asyncio.gather(*consumers)


if __name__ == "__main__":
    asyncio.run(main())
