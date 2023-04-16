"""
This script demonstrates the producer-consumer pattern using Python threads.
It simulates a scenario where producers create items and put them into a shared buffer,
and consumers take items from the buffer and process them.

The shared buffer is represented by a Python Queue, which is thread-safe by design.
"""

import random
import time
from queue import Queue
from threading import Thread


def producer(buffer, id):
    for _ in range(10):
        item = random.randint(1, 100)
        buffer.put(item)
        print(f"Producer {id} produced item {item}")
        time.sleep(random.uniform(0.1, 0.5))


def consumer(buffer, id):
    while True:
        item = buffer.get()
        if item is None:
            break
        print(f"Consumer {id} consumed item {item}")
        time.sleep(random.uniform(0.1, 0.5))


if __name__ == "__main__":
    buffer = Queue(maxsize=10)
    num_producers = 3
    num_consumers = 3

    producers = [Thread(target=producer, args=(buffer, i + 1)) for i in range(num_producers)]
    consumers = [Thread(target=consumer, args=(buffer, i + 1)) for i in range(num_consumers)]

    for producer in producers:
        producer.start()

    for consumer in consumers:
        consumer.start()

    for producer in producers:
        producer.join()

    for _ in range(num_consumers):
        buffer.put(None)

    for consumer in consumers:
        consumer.join()
