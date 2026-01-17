"""
Periodic Task Scheduler Example

This script demonstrates implementing a scheduler that runs a function
at fixed intervals using threads. This pattern is useful for:
- Polling or heartbeat operations
- Periodic data collection
- Scheduled cleanup tasks
- Status reporting

Key Concepts:
- Use a flag to control thread lifecycle
- Thread runs in a loop, sleeping between executions
- join() ensures clean shutdown

Note: For production use, consider the 'schedule' library or APScheduler
for more robust scheduling with error handling and missed execution recovery.
"""

import time
from threading import Thread


class Scheduler:
    """
    A scheduler that repeatedly executes a function at fixed intervals.

    The scheduler runs the function in a separate thread and can be
    stopped gracefully using the stop() method.
    """

    def __init__(self, interval, function, *args, **kwargs):
        """
        Initialize and start the scheduler.

        Args:
            interval: Seconds between function executions
            function: The function to execute periodically
            *args: Positional arguments for the function
            **kwargs: Keyword arguments for the function
        """
        self._running = True
        self._thread = Thread(target=self._run, args=(interval, function, args, kwargs))
        self._thread.start()

    def _run(self, interval, function, args, kwargs):
        """Internal method that runs the scheduling loop."""
        while self._running:
            function(*args, **kwargs)
            time.sleep(interval)

    def stop(self):
        """
        Stop scheduling new function calls.

        If a function call is in progress, it will complete before stopping.
        This method blocks until the scheduler thread terminates.
        """
        self._running = False
        self._thread.join()


def sample_function(message):
    """Example function to be scheduled."""
    print(f"[{time.strftime('%H:%M:%S')}] {message}")


def main():
    print("Starting scheduler...")
    scheduler = Scheduler(1, sample_function, "It works!")

    # Let it run for 10 seconds
    time.sleep(10)

    print("Stopping scheduler...")
    scheduler.stop()
    print("Scheduler stopped.")


if __name__ == "__main__":
    main()
