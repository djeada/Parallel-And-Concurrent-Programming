"""
Pipe Communication Example

This script demonstrates inter-process communication using multiprocessing.Pipe.
Pipes provide a low-level, bidirectional communication channel between exactly
two processes.

Key Concepts:
- Pipe() returns two connection objects (endpoints)
- Each endpoint can send() and recv() data
- Lower overhead than Queue for two-process communication
- EOFError is raised when the other end closes

Use Cases:
- Direct communication between parent and child process
- Simple request-response patterns
- When you need bidirectional communication

For multi-producer/multi-consumer scenarios, use Queue instead.
Pipes are more efficient but limited to exactly two endpoints.
"""

import random
import time
from multiprocessing import Process, Pipe


def producer(conn):
    """Produce items and send them through the pipe."""
    random.seed(time.time())

    for _ in range(5):
        item = random.randint(1, 10)
        print(f"Producer: Adding {item} to the pipe")
        conn.send(item)
        time.sleep(random.uniform(0.5, 1.5))

    conn.close()  # Close connection to signal end of data


def consumer(conn):
    """Receive and process items from the pipe."""
    while True:
        try:
            item = conn.recv()
            print(f"Consumer: Processing {item}")
            time.sleep(random.uniform(0.5, 1.5))
        except EOFError:
            # Other end of pipe was closed
            break

    conn.close()


def main():
    # Create a pipe with two endpoints
    parent_conn, child_conn = Pipe()

    # Create processes with their respective pipe endpoints
    producer_process = Process(target=producer, args=(parent_conn,))
    consumer_process = Process(target=consumer, args=(child_conn,))

    producer_process.start()
    consumer_process.start()

    producer_process.join()
    parent_conn.close()  # Close our reference to allow EOFError in consumer

    consumer_process.join()
    child_conn.close()


if __name__ == "__main__":
    main()
