"""
This script demonstrates the use of a mutex (short for "mutual exclusion") to protect shared resources in a multi-threaded application.
It shows how a mutex can be used to ensure that only one thread at a time has access to a critical section of code.

The script creates 4 threads, each of which attempts to print its thread id 100 times with a short pause between each print. Without a mutex,
the threads would interleave their prints, resulting in an unpredictable output. By using a mutex to synchronize access to the print
statement, we ensure that each thread finishes its printing before the next thread starts, providing a predictable output.

To see the effect of the mutex, comment out the lines that acquire and release the mutex, and observe the change in the output.
"""

import time
from threading import Thread, Lock

mutex = Lock()


def foo(id):
    with mutex:  # comment out this line of code to test what happens without the mutex
        for _ in range(100):
            print("Thread id:", id)
            time.sleep(0.05)
    # The mutex is automatically released when the block of code under the "with" statement is exited


if __name__ == "__main__":
    print("MAIN FUNCTION STARTS")
    threads = []
    for i in range(4):
        threads.append(Thread(target=foo, args=(i,)))
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    print("THE END")
