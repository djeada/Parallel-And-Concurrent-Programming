"""
This script demonstrates the use of the multiprocessing module in Python to establish communication
between multiple processes using a Queue. The Queue allows for safe inter-process communication
by providing a simple way to pass data between processes. In this example, we create a producer process
that generates data and puts it into the Queue, and a consumer process that reads the data from the Queue.

When working with multiple processes, it is important to ensure that data is safely shared between them.
The multiprocessing module provides a Queue class that can be used to achieve this. The Queue uses a
lock to ensure that only one process can access it at a time, preventing race conditions and data corruption.

In this example, we demonstrate the use of a Queue to share data between a producer and a consumer process.
The producer generates data and puts it into the Queue, while the consumer reads the data from the Queue and
processes it. This pattern is useful when the producer and consumer processes work at different rates or when
you want to separate data generation from data processing.
"""

import multiprocessing
import time
import random

def producer(queue):
    for i in range(5):
        item = random.randint(1, 10)
        print(f"Producer: Adding {item} to the queue")
        queue.put(item)
        time.sleep(random.uniform(0.5, 1))

def consumer(queue):
    while True:
        item = queue.get()
        if item is None:
            break

        print(f"Consumer: Processing {item}")
        time.sleep(random.uniform(0.5, 1))

def main():
    # Create a Queue to share data between the producer and consumer processes
    queue = multiprocessing.Queue()

    # Create the producer and consumer processes
    producer_process = multiprocessing.Process(target=producer, args=(queue,))
    consumer_process = multiprocessing.Process(target=consumer, args=(queue,))

    # Start the producer and consumer processes
    producer_process.start()
    consumer_process.start()

    # Wait for the producer process to finish
    producer_process.join()

    # Add a sentinel value to the queue to signal the consumer process to exit
    queue.put(None)

    # Wait for the consumer process to finish
    consumer_process.join()

if __name__ == "__main__":
    main()
