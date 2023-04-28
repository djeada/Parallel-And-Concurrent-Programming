"""
This program demonstrates the concept of orphan processes in Python. An orphan process
is a child process whose parent process has terminated before the child process. In most
operating systems, including Linux, when the parent process terminates, its orphaned
children are reparented to the init or systemd process (PID 1).

In this example, we create a child process that outlives its parent process, becoming
an orphan. After the parent process exits, you can check the process list (using
commands like 'ps' or 'top') to see that the child process is still running and has
been reparented to the init or systemd process (PID 1).
"""

import os
import time


def main():
    pid = os.fork()

    if pid == 0:
        # Child process
        print(f"Child process (PID: {os.getpid()}) started.")
        time.sleep(10)
        print(f"Child process (PID: {os.getpid()}) finished.")
    else:
        # Parent process
        print(f"Parent process (PID: {os.getpid()}) started.")
        print(f"Parent process (PID: {os.getpid()}) is exiting, leaving the child process orphaned.")


if __name__ == "__main__":
    main()
