"""
This script demonstrates a deadlock situation using the multiprocessing module in Python.
A deadlock occurs when two processes are waiting for each other to release a lock, creating
a cycle that never ends. In this example, two processes 'process_a' and 'process_b' each
acquire a lock and then try to acquire the other lock, causing a deadlock.
"""

import time
import multiprocessing as mp


def process_a(lock_a, lock_b):
    with lock_a:
        print("Process A acquired lock_a")
        time.sleep(1)
        print("Process A trying to acquire lock_b")
        with lock_b:
            print("Process A acquired lock_b")


def process_b(lock_a, lock_b):
    with lock_b:
        print("Process B acquired lock_b")
        time.sleep(1)
        print("Process B trying to acquire lock_a")
        with lock_a:
            print("Process B acquired lock_a")


if __name__ == "__main__":
    lock_a = mp.Lock()
    lock_b = mp.Lock()

    p_a = mp.Process(target=process_a, args=(lock_a, lock_b))
    p_b = mp.Process(target=process_b, args=(lock_a, lock_b))

    p_a.start()
    p_b.start()

    p_a.join()
    p_b.join()

    print("Main process finished")

