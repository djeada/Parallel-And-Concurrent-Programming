"""
Async Producer-Consumer Example: Crawling URLs with limited workers

Problem:
Imagine we have many URLs to download.

Bad approach:
- Start one task per URL immediately
- If there are thousands of URLs, we may overload the server
- We may also overload our own program with too many active tasks

Better approach:
- Producers discover or create work
- They place work into an asyncio.Queue
- Consumers take work from the queue and process it
- The queue decouples "finding work" from "doing work"
- A bounded queue adds backpressure

Real-world use cases:
- Web crawlers
- Background job workers
- Image/video processing pipelines
- Log processing
- Message queues
- Database import/export jobs

Key idea:
The queue is the buffer between fast producers and slower consumers.
"""

import asyncio
import random


async def fake_download(url: str) -> str:
    """
    Simulate a slow network request.
    """
    print(f"Downloading {url}...")
    await asyncio.sleep(random.uniform(0.5, 1.5))
    return f"<html>{url}</html>"


async def save_page(url: str, html: str) -> None:
    """
    Simulate saving downloaded data.
    """
    await asyncio.sleep(random.uniform(0.2, 0.6))
    print(f"Saved {url} ({len(html)} bytes)")


async def producer(queue: asyncio.Queue[str], producer_id: int, urls: list[str]) -> None:
    """
    Producer adds URLs to the queue.

    It does not download them directly.
    It only creates work for consumers.
    """
    for url in urls:
        print(f"Producer {producer_id}: discovered {url}")

        # If the queue is full, this await pauses the producer.
        # That is backpressure: producers slow down when consumers cannot keep up.
        await queue.put(url)

        print(f"Producer {producer_id}: queued {url}")

    print(f"Producer {producer_id}: finished")


async def consumer(queue: asyncio.Queue[str], consumer_id: int) -> None:
    """
    Consumer processes URLs from the queue.

    It keeps working until it receives a None sentinel.
    """
    while True:
        url = await queue.get()

        try:
            if url is None:
                print(f"Consumer {consumer_id}: shutting down")
                return

            print(f"Consumer {consumer_id}: processing {url}")

            html = await fake_download(url)
            await save_page(url, html)

            print(f"Consumer {consumer_id}: finished {url}")

        finally:
            # Required when queue.join() is used.
            # This tells the queue that one queued item has been fully handled.
            queue.task_done()


async def main():
    """
    Coordinate producers and consumers.
    """

    # Bounded queue.
    #
    # This allows only 3 waiting URLs at a time.
    # If producers discover URLs faster than consumers process them,
    # producers will pause at queue.put().
    queue: asyncio.Queue[str | None] = asyncio.Queue(maxsize=3)

    urls_from_source_1 = [
        "https://example.com/page-1",
        "https://example.com/page-2",
        "https://example.com/page-3",
        "https://example.com/page-4",
    ]

    urls_from_source_2 = [
        "https://example.com/product-1",
        "https://example.com/product-2",
        "https://example.com/product-3",
        "https://example.com/product-4",
    ]

    producers = [
        asyncio.create_task(producer(queue, 1, urls_from_source_1)),
        asyncio.create_task(producer(queue, 2, urls_from_source_2)),
    ]

    consumers = [
        asyncio.create_task(consumer(queue, 1)),
        asyncio.create_task(consumer(queue, 2)),
        asyncio.create_task(consumer(queue, 3)),
    ]

    # Wait until producers have discovered all work.
    await asyncio.gather(*producers)

    # Wait until all real URLs have been processed.
    await queue.join()

    # Stop consumers.
    #
    # Each consumer needs one sentinel because each sentinel stops one consumer.
    for _ in consumers:
        await queue.put(None)

    # Wait until the sentinel values have also been received.
    await queue.join()

    # Wait for the consumer tasks to actually exit.
    await asyncio.gather(*consumers)


if __name__ == "__main__":
    asyncio.run(main())
