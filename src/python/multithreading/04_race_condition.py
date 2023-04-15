"""
This script demonstrates the concept of a race condition in multithreading.
Multiple threads access and modify a shared global variable `COUNTER` without
any synchronization mechanism, leading to unpredictable results.

Each thread runs a function `foo`, which multiplies the global variable `COUNTER`
by the thread's multiplier in 10 iterations. Due to the race condition, the final
value of `COUNTER` depends on the order and timing of the threads' execution.
"""

from threading import Thread
import time

COUNTER = 1


def foo(multiplier):
    global COUNTER
    for _ in range(10):
        time.sleep(0.1)
        local_counter = COUNTER
        local_counter *= multiplier
        print(f"The COUNTER gets multiplied by {multiplier}")
        COUNTER = local_counter


if __name__ == "__main__":
    threads = []
    for i in range(10):
        threads.append(Thread(target=foo, args=(i + 1,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print(f"The final value of COUNTER is: {COUNTER}")
