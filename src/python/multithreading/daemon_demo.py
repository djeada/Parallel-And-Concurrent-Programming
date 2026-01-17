"""
Daemon Thread Example

This script demonstrates the behavior of daemon threads in Python.
Daemon threads are automatically terminated when the main program exits,
unlike regular threads which keep the program running until they complete.

Key Concepts:
- daemon=True makes a thread a daemon thread
- Daemon threads are abruptly stopped when the main thread exits
- Non-daemon threads prevent the program from exiting until they complete
- Use daemon threads for background tasks that shouldn't block shutdown

Use Cases:
- Background logging
- Periodic cleanup tasks
- Monitoring threads
- Any background task that should not prevent program exit

Try This:
Uncomment t.join() to see the difference - with join(), the main thread
waits for the daemon thread to complete before exiting.
"""

import threading
import time


def worker():
    """Worker function that simulates ongoing background work."""
    for i in range(5):
        print(f"Worker running... {i}")
        time.sleep(1)


def main():
    print("Main thread starting")

    # Create a daemon thread
    t = threading.Thread(target=worker, daemon=True)
    t.start()

    # Uncomment the next line to wait for the daemon thread to complete
    # t.join()

    print("Main thread finished")


if __name__ == "__main__":
    main()
