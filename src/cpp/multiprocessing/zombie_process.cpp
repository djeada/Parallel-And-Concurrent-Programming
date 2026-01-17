/**
 * Zombie Process
 *
 * This example demonstrates zombie processes - child processes that
 * have terminated but whose exit status hasn't been collected.
 *
 * Key concepts:
 * - When a child exits, it becomes a "zombie" until parent calls wait()
 * - Zombie processes consume a slot in the process table
 * - They appear with status 'Z' or 'Z+' in 'ps' output
 * - Parent MUST call wait()/waitpid() to reap zombies
 * - Too many zombies can exhaust the process table
 *
 * Prevention:
 * - Always wait() for child processes
 * - Use SIGCHLD handler with wait()
 * - Set SIGCHLD to SIG_IGN (Linux-specific)
 *
 * WARNING: This example intentionally creates a zombie for demonstration.
 * To observe: Run this and use 'ps aux | grep Z' during the 10-second sleep.
 */

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "fork() failed\n";
        return 1;
    }

    if (pid == 0) {
        // Child process
        std::cout << "Child: PID=" << getpid() << "\n";
        std::cout << "Child: Exiting immediately\n";
        // Use _exit() instead of exit() to avoid calling atexit handlers
        // and flushing stdio buffers that were inherited from parent
        _exit(42);
    } else {
        // Parent process
        std::cout << "Parent: PID=" << getpid()
                  << ", Child PID=" << pid << "\n";
        std::cout << "Parent: NOT calling wait() - child becomes zombie\n";
        std::cout << "Parent: Check 'ps aux | grep Z' in the next 10 seconds\n";

        sleep(10);  // During this time, child is a zombie

        // Now reap the zombie
        int status;
        waitpid(pid, &status, 0);
        std::cout << "Parent: Finally reaped child, exit status="
                  << WEXITSTATUS(status) << "\n";
        std::cout << "Parent: Zombie is now gone\n";
    }

    return 0;
}
