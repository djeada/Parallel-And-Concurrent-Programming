"""
A simple program to demonstrate a scheduler that runs a function every n seconds.
The scheduler runs the function in a separate thread and can be stopped after a specified duration.
"""

import time
from threading import Thread


class Scheduler:
    """
    A scheduler class that repeatedly executes a given function every n seconds.
    """

    def __init__(self, n, function, *args, **kwargs):
        """
        Initialize the scheduler with the interval n, the function to execute, and any arguments or keyword arguments.

        :param n: The interval in seconds between executions of the function.
        :param function: The function to execute.
        :param args: Positional arguments for the function.
        :param kwargs: Keyword arguments for the function.
        """

        def target():
            while self.flag:
                function(*args, **kwargs)
                time.sleep(n)

        self.flag = True
        self.thread = Thread(target=target)
        self.thread.start()

    def stop(self):
        """
        Stop scheduling new function calls.
        If the previous function call has not yet completed, it will not be terminated.
        """
        self.flag = False
        self.thread.join()


def sample_function(message):
    print(message)


if __name__ == "__main__":
    scheduler = Scheduler(1, sample_function, "It works!")

    # Wait 10 seconds and stop the scheduler
    time.sleep(10)
    scheduler.stop()
