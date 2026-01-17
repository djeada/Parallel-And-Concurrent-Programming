"""
Zombie Process Example

This script demonstrates the concept of zombie processes. A zombie process
is a child process that has terminated but hasn't been "reaped" by its parent
(i.e., the parent hasn't called wait() to collect its exit status).

Key Concepts:
- Zombie processes have completed execution but still exist in process table
- They retain their process ID and exit status until parent calls wait()
- Shown as 'Z' or 'Z+' status in ps output
- Consume minimal resources (just a process table entry)
- Too many zombies can exhaust the process table

Prevention:
- Always use wait() or waitpid() to reap child processes
- Use signal handler for SIGCHLD
- In Python, use process.join() or os.waitpid()

WARNING: This example intentionally creates a zombie process for demonstration.
The parent sleeps instead of reaping the child, allowing you to observe
the zombie state using 'ps aux | grep Z'.

Note: This uses os.fork() which is only available on Unix-like systems.
"""

import os
import time
import sys


def main():
    pid = os.fork()

    if pid == 0:
        # Child process
        print(f"Child process (PID: {os.getpid()}) started.")
        time.sleep(2)
        print(f"Child process (PID: {os.getpid()}) finished.")
        sys.exit(0)  # Child terminates but parent doesn't reap it
    else:
        # Parent process
        print(f"Parent process (PID: {os.getpid()}) started.")
        print(
            f"Parent process (PID: {os.getpid()}) is NOT waiting "
            "for the child process to terminate."
        )
        print(f"Child process (PID: {pid}) will become a zombie after termination.")
        print("Check with: ps aux | grep Z")
        time.sleep(10)  # Parent sleeps instead of reaping child
        print(f"Parent process (PID: {os.getpid()}) finished.")


if __name__ == "__main__":
    main()
