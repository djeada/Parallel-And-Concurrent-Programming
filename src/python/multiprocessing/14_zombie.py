"""
This program demonstrates the concept of zombie processes in Python. A zombie process is
a child process that has terminated but has not been reaped by its parent process. The
operating system keeps an entry in the process table for the terminated child process,
holding information about its termination status until the parent process collects this
information using a system call (usually wait() or waitpid()).

In this example, we create a child process that terminates but isn't reaped by its
parent, becoming a zombie. After the child process exits, you can check the process
list (using commands like 'ps' or 'top') to see that the child process is in a 'zombie'
state, denoted by the status 'Z+'.

WARNING: This example intentionally creates a zombie process. You should manually kill
the parent process after running the script to avoid having a zombie process lingering
on your system.
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
        sys.exit()

    else:
        # Parent process
        print(f"Parent process (PID: {os.getpid()}) started.")
        print(
            f"Parent process (PID: {os.getpid()}) is NOT waiting for the child process to terminate."
        )
        print(f"Child process (PID: {pid}) will become a zombie after termination.")
        time.sleep(10)
        print(f"Parent process (PID: {os.getpid()}) finished.")


if __name__ == "__main__":
    main()
