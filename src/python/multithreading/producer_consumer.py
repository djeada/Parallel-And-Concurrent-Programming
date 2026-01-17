"""
Producer-Consumer Pattern Example

This script demonstrates the classic producer-consumer pattern using Python threads.
Producers create items and add them to a shared buffer, while consumers take items
from the buffer and process them.

Key Concepts:
- queue.Queue provides a thread-safe FIFO queue
- Producers put() items, consumers get() items
- Queue handles all synchronization internally
- Use None as a sentinel value to signal consumers to stop

This pattern is fundamental for:
- Task distribution systems
- Data processing pipelines
- Event-driven architectures
- Load balancing

The Queue's maxsize parameter can be used to implement backpressure,
causing producers to block when the queue is full.
"""

import random
import time
from queue import Queue
from threading import Thread


def producer(buffer, producer_id):
    """Produce 10 random items and add them to the buffer."""
    for _ in range(10):
        item = random.randint(1, 100)
        buffer.put(item)
        print(f"Producer {producer_id} produced item {item}")
        time.sleep(random.uniform(0.1, 0.5))


def consumer(buffer, consumer_id):
    """Consume items from the buffer until receiving None sentinel."""
    while True:
        item = buffer.get()
        if item is None:  # Sentinel value signals shutdown
            break
        print(f"Consumer {consumer_id} consumed item {item}")
        time.sleep(random.uniform(0.1, 0.5))


def main():
    buffer = Queue(maxsize=10)  # Bounded buffer with backpressure
    num_producers = 3
    num_consumers = 3

    producers = [
        Thread(target=producer, args=(buffer, i + 1)) for i in range(num_producers)
    ]
    consumers = [
        Thread(target=consumer, args=(buffer, i + 1)) for i in range(num_consumers)
    ]

    # Start all threads
    for p in producers:
        p.start()
    for c in consumers:
        c.start()

    # Wait for producers to finish
    for p in producers:
        p.join()

    # Signal consumers to stop by sending sentinel values
    for _ in range(num_consumers):
        buffer.put(None)

    # Wait for consumers to finish
    for c in consumers:
        c.join()


if __name__ == "__main__":
    main()
