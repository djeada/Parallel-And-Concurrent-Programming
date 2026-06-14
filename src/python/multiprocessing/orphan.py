"""
Orphan Process Example

This script demonstrates the concept of orphan processes. An orphan process
is a child process whose parent has terminated before the child completes.

Key Concepts:
- When a parent exits, orphaned children are reparented to init/systemd (PID 1)
- Orphan processes continue to run normally
- They are "adopted" by the system's init process
- This is different from zombie processes (which have terminated but not been reaped)

Behavior:
1. Parent spawns child process
2. Parent exits immediately
3. Child continues running, now orphaned
4. OS reparents child to init (PID 1)
5. When child eventually exits, init reaps it

Use 'ps -ef | grep python' to observe the orphaned child process
and verify its parent PID has changed to 1.

ANTI-PATTERN DEMO:
This is intentionally not a normal application pattern. In production, prefer
explicit process ownership: keep the parent alive, call join(), and provide a
clear shutdown path for child processes.

Note: This uses os.fork() which is only available on Unix-like systems.
"""

import os
import time


def main():
    pid = os.fork()

    if pid == 0:
        # Child process
        print(f"Child process (PID: {os.getpid()}) started.")
        print(f"Child's parent PID: {os.getppid()}")
        time.sleep(10)  # Child outlives parent
        print(f"Child process (PID: {os.getpid()}) finished.")
        print(f"Child's parent PID after orphaning: {os.getppid()}")
    else:
        # Parent process
        print(f"Parent process (PID: {os.getpid()}) started.")
        print(
            f"Parent process (PID: {os.getpid()}) is exiting, "
            "leaving the child process orphaned.",
            flush=True,
        )
        os._exit(0)  # Parent exits immediately, orphaning the child


if __name__ == "__main__":
    main()
