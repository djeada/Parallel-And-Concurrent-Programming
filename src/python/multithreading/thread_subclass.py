"""
Thread Subclass Example

This script demonstrates how to create custom thread classes by subclassing
threading.Thread. This approach is useful when you need:
- Encapsulated thread behavior
- Custom initialization logic
- State management within the thread

Key Concepts:
- Subclass Thread and override the run() method
- Call super().__init__() in __init__ to properly initialize the Thread
- Use join() to wait for threads to complete

Note: Starting a thread in __init__ (as shown here) is a valid pattern
but should be used carefully - the thread begins executing immediately
upon instantiation.
"""

import time
from threading import Thread


class MyThread(Thread):
    """Custom thread class that wraps a function and logs execution."""

    def __init__(self, function, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.function = function
        self.start()  # Thread starts immediately upon creation

    def run(self):
        """Execute the wrapped function with logging."""
        print(f"{self.function.__name__} started")
        self.function()
        print(f"{self.function.__name__} finished")


def my_function():
    """Example function that simulates a long-running task."""
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
