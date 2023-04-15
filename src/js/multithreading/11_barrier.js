"""
This script demonstrates the use of the threading.Barrier class in Python
to synchronize multiple threads at a specific point in the execution. The worker
threads perform some work, wait at the barrier, and then continue after all
threads have reached the barrier.
"""

import threading
import time
import random

def worker(barrier, thread_id):
    print(f"Thread {thread_id} is starting...")
    time.sleep(random.uniform(1, 3))  # Simulate some work
    print(f"Thread {thread_id} is waiting at the barrier...")

    barrier.wait()  # Wait for all threads to reach the barrier

    print(f"Thread {thread_id} is resuming after the barrier...")

def main():
    num_threads = 5
    barrier = threading.Barrier(num_threads)

    threads = [threading.Thread(target=worker, args=(barrier, i)) for i in range(num_threads)]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print("All threads have passed the barrier.")

if __name__ == "__main__":
    main()

