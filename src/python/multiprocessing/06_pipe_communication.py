"""
This program demonstrates the use of pipes to establish communication between two processes in Python.
A pipe is a low-level communication primitive that provides a way for two processes to exchange data.
In this example, we create a producer process that generates data and writes it into the pipe, and a consumer
process that reads the data from the pipe.

When working with multiple processes, it is important to ensure that data is safely shared between them.
Pipes provide a way to achieve this. They can be used to send data between a producer and a consumer process.
The producer generates data and writes it into the pipe, while the consumer reads the data from the pipe and processes it.
This pattern is useful when the producer and consumer processes work at different rates or when
you want to separate data generation from data processing.
"""

import random
import time
from multiprocessing import Process, Pipe


def producer(conn):
    random.seed(time.time())

    for _ in range(5):
        item = random.randint(1, 10)
        print(f"Producer: Adding {item} to the pipe")
        conn.send(item)
        time.sleep(random.uniform(0.5, 1.5))

    conn.close()


def consumer(conn):
    while True:
        try:
            item = conn.recv()
            print(f"Consumer: Processing {item}")
            time.sleep(random.uniform(0.5, 1.5))
        except EOFError:
            break

    conn.close()


def main():
    parent_conn, child_conn = Pipe()

    producer_process = Process(target=producer, args=(parent_conn,))
    consumer_process = Process(target=consumer, args=(child_conn,))

    producer_process.start()
    consumer_process.start()

    producer_process.join()
    parent_conn.close()

    consumer_process.join()
    child_conn.close()


if __name__ == "__main__":
    main()
