"""
This script demonstrates the concept of synchronization in multithreading using a semaphore.
Multiple threads access and modify a shared global variable `COUNTER` with a synchronization
mechanism (a semaphore), leading to predictable results.

Each thread runs a function `foo`, which multiplies the global variable `COUNTER`
by the thread's multiplier in 10 iterations. Due to the use of the semaphore, the final
value of `COUNTER` will be the same in every execution.
"""

from threading import Thread, Semaphore
import time

COUNTER = 1
semaphore = Semaphore(1)


def foo(multiplier):
    global COUNTER
    for _ in range(10):
        time.sleep(0.1)
        with semaphore:
            local_counter = COUNTER
            local_counter *= multiplier
            print(f"The COUNTER gets multiplied by {multiplier}")
            COUNTER = local_counter


if __name__ == "__main__":
    threads = []
    for i in range(3):
        threads.append(Thread(target=foo, args=(i + 1,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print(f"The final value of COUNTER is: {COUNTER}")
