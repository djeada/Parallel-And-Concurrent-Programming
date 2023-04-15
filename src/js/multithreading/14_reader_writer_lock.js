"""
This script demonstrates the use of a Reader-Writer lock to manage concurrent access
to shared data in a multi-threaded environment, allowing multiple readers to access
the data simultaneously while preventing concurrent access by readers and writers.
"""

import threading
import time
import random

class SharedData:
    def __init__(self):
        self.data = 0
        self.lock = threading.RLock()
        self.readers = 0
        self.readers_lock = threading.Lock()

    def read(self, thread_id):
        with self.readers_lock:
            self.readers += 1
            if self.readers == 1:
                self.lock.acquire()

        print(f"Reader {thread_id} reads data: {self.data}")
        time.sleep(random.uniform(0.5, 1))  # Simulate some work

        with self.readers_lock:
            self.readers -= 1
            if self.readers == 0:
                self.lock.release()

    def write(self, thread_id):
        with self.lock:
            print(f"Writer {thread_id} is writing...")
            self.data += 1
            time.sleep(random.uniform(0.5, 1))  # Simulate some work
            print(f"Writer {thread_id} finished writing. New data: {self.data}")

def reader(shared_data, thread_id):
    while True:
        shared_data.read(thread_id)
        time.sleep(random.uniform(1, 2))

def writer(shared_data, thread_id):
    while True:
        shared_data.write(thread_id)
        time.sleep(random.uniform(1, 2))

def main():
    shared_data = SharedData()

    num_readers = 3
    num_writers = 2

    threads = []

    for i in range(num_readers):
        t = threading.Thread(target=reader, args=(shared_data, i))
        t.start()
        threads.append(t)

    for i in range(num_writers):
        t = threading.Thread(target=writer, args=(shared_data, i))
        t.start()
        threads.append(t)

    for t in threads:
        t.join()

if __name__ == "__main__":
    main()

