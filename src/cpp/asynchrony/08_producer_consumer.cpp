import asyncio
import random


async def producer(queue):
    while True:
        item = random.randint(1, 10)
        print(f"Producing {item}")
        await queue.put(item)
        await asyncio.sleep(random.random())


async def consumer(queue):
    while True:
        item = await queue.get()
        print(f"Consuming {item}")
        await asyncio.sleep(random.random())


async def main():
    queue = asyncio.Queue(maxsize=5)

    producers = [asyncio.create_task(producer(queue)) for _ in range(2)]
    consumers = [asyncio.create_task(consumer(queue)) for _ in range(4)]

    await asyncio.gather(*producers, *consumers)


if __name__ == "__main__":
    asyncio.run(main())
