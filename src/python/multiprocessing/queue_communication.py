"""
Queue Communication Example

This script demonstrates inter-process communication using multiprocessing.Queue.
Queues provide a thread-safe and process-safe way to exchange data between
processes using a FIFO (First-In-First-Out) mechanism.

Key Concepts:
- Queue handles all synchronization internally (thread/process-safe)
- put() adds items, get() retrieves and removes items
- Use None as a sentinel value to signal termination
- Queue can have a maximum size (backpressure mechanism)

Use Cases:
- Producer-consumer patterns across processes
- Task distribution systems
- Data pipelines with multiple processing stages

Alternative: Use Pipe for bidirectional communication between exactly
two processes with lower overhead.
"""

import multiprocessing
import time
import random


def producer(queue):
    """Produce items and add them to the queue."""
    for i in range(5):
        item = random.randint(1, 10)
        print(f"Producer: Adding {item} to the queue")
        queue.put(item)
        time.sleep(random.uniform(0.5, 1))


def consumer(queue):
    """Consume items from the queue until receiving sentinel value."""
    while True:
        item = queue.get()
        if item is None:  # Sentinel value signals shutdown
            break
        print(f"Consumer: Processing {item}")
        time.sleep(random.uniform(0.5, 1))


def main():
    # Create a Queue to share data between processes
    queue = multiprocessing.Queue()

    # Create producer and consumer processes
    producer_process = multiprocessing.Process(target=producer, args=(queue,))
    consumer_process = multiprocessing.Process(target=consumer, args=(queue,))

    # Start both processes
    producer_process.start()
    consumer_process.start()

    # Wait for producer to finish
    producer_process.join()

    # Signal consumer to stop
    queue.put(None)

    # Wait for consumer to finish
    consumer_process.join()


if __name__ == "__main__":
    main()
