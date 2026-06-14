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

ANTI-PATTERN DEMO:
This example intentionally creates a zombie process for a few seconds. The
parent sleeps instead of immediately reaping the child so you can observe the
zombie state using 'ps -o pid,ppid,state,cmd -p <child_pid>'. It then calls
os.waitpid() so the demo does not leave a zombie behind.

Note: This uses os.fork() which is only available on Unix-like systems.
"""

import os
import time
import sys


def main():
    pid = os.fork()

    if pid == 0:
        # Child process
        print(f"Child process (PID: {os.getpid()}) started.", flush=True)
        time.sleep(2)
        print(f"Child process (PID: {os.getpid()}) finished.", flush=True)
        sys.exit()  # Child terminates but parent doesn't reap it
    else:
        # Parent process
        print(f"Parent process (PID: {os.getpid()}) started.", flush=True)
        print(
            f"Parent process (PID: {os.getpid()}) is NOT waiting "
            "for the child process to terminate.",
            flush=True,
        )
        print(
            f"Child process (PID: {pid}) will become a zombie after termination.",
            flush=True,
        )
        print(f"Check with: ps -o pid,ppid,state,cmd -p {pid}", flush=True)
        time.sleep(5)  # Anti-pattern window: parent delays reaping the child
        os.waitpid(pid, 0)
        print(f"Parent process reaped child process (PID: {pid}).", flush=True)
        print(f"Parent process (PID: {os.getpid()}) finished.", flush=True)


if __name__ == "__main__":
    main()
