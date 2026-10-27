/**
 * LIFECYCLE OBSERVATION: intentional parent exit, not a worker ownership model.
 * Start with basic_process.cpp for scoped child management.
 *
 * Orphan Process
 *
 * This example demonstrates orphan processes - child processes whose
 * parent has terminated before them.
 *
 * Key concepts:
 * - When a parent exits, its children become "orphans"
 * - The init process (PID 1) or systemd adopts orphaned children
 * - Orphaned children continue running normally
 * - The new parent may be PID 1 or a configured subreaper
 *
 * Orphan processes are not inherently bad - they're properly managed
 * by the OS. However, they can make process hierarchy tracking harder.
 *
 * To observe: Run this program and use 'ps -ef | grep orphan' to
 * see the child process get reparented to init.
 */

#include <iostream>
#include <unistd.h>

int main() {
    std::cout.flush();
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "fork() failed\n";
        return 1;
    }

    if (pid == 0) {
        // Child process
        std::cout << "Child: PID=" << getpid()
                  << ", Parent PID=" << getppid() << "\n";

        sleep(2);  // Wait for parent to exit

        // After parent exit, the kernel reparents to init or a subreaper.
        std::cout << "Child: After delay, Parent PID=" << getppid() << "\n";
        std::cout << "Child: The kernel has reparented me\n";

        sleep(3);
        std::cout << "Child: Exiting\n";
    } else {
        // Parent process
        std::cout << "Parent: PID=" << getpid()
                  << ", Child PID=" << pid << "\n";
        std::cout << "Parent: Exiting immediately (not waiting for child)\n";
        // Parent exits without waiting - child becomes orphan
    }

    return 0;
}
