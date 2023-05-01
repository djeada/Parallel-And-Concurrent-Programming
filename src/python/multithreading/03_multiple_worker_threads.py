"""
This script demonstrates the creation of multiple threads that execute different functions concurrently.
It creates two sets of threads, one set running the 'foo' function and the other set running the 'bar' function.
Each function has a different execution time, and each thread is given an ID to track its progress.
"""

import time
from threading import Thread


def foo(id):
    print(f"Function foo() starts, Thread id: {id}")
    time.sleep(1)
    print(f"Function foo() finishes, Thread id: {id}")


def bar(id):
    print(f"Function bar() starts, Thread id: {id}")
    time.sleep(0.1)
    print(f"Function bar() finishes, Thread id: {id}")


if __name__ == "__main__":
    print("Main thread starts")

    threads = []
    for i in range(5):
        threads.append(Thread(target=foo, args=(i,)))

    for i in range(5):
        threads.append(Thread(target=bar, args=(i,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print("Main thread finishes")
