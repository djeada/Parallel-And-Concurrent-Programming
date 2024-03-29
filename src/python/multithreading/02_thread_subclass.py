"""
This module demonstrates the usage of custom threads in Python by implementing a
`MyThread` class, which is a subclass of the built-in `Thread` class.

The `MyThread` class is initialized with a function and automatically starts the
thread upon instantiation. It then runs the given function and prints the start
and finish status of the function.

Two threads are created in the `main()` function, one with a custom function called
`my_function` and another with a lambda function. Both threads are joined, and the
main thread waits for them to finish before terminating.
"""

import time
from threading import Thread


class MyThread(Thread):
    def __init__(self, function, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.function = function
        self.start()

    def run(self):
        print(f"{self.function.__name__} started")
        self.function()
        print(f"{self.function.__name__} finished")


def my_function():
    print("my_function goes to sleep")
    time.sleep(5)
    print("my_function wakes up")


def main():
    # Start threads with different functions
    thread_a = MyThread(my_function)
    thread_b = MyThread(lambda: print("it's me, the lambda function"))

    # Wait for both threads to finish
    for thread in (thread_a, thread_b):
        thread.join()

    print("Main thread finished")


if __name__ == "__main__":
    main()
