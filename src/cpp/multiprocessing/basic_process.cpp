/**
 * Basic Process Creation with fork()
 *
 * This example demonstrates the fundamental process creation mechanism
 * in Unix/Linux systems using fork().
 *
 * Key concepts:
 * - fork() creates a new process by duplicating the calling process
 * - Returns 0 in the child process, positive PID in the parent
 * - Child process has its own memory space (copy-on-write)
 * - waitpid() ensures parent waits for child to complete
 *
 * Unlike threads, processes have completely separate memory spaces
 * and require IPC (Inter-Process Communication) to share data.
 */

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void child_work() {
    std::cout << "Child process started\n";
    std::cout << "  Child PID: " << getpid() << "\n";
    std::cout << "  Parent PID: " << getppid() << "\n";
    std::cout << "Child process finished\n";
}

int main() {
    std::cout << "Main process started (PID: " << getpid() << ")\n";

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "fork() failed\n";
        return 1;
    }

    if (pid == 0) {
        // Child process
        child_work();
    } else {
        // Parent process
        std::cout << "Parent waiting for child (PID: " << pid << ")...\n";
        waitpid(pid, nullptr, 0);
        std::cout << "Parent process finished\n";
    }

    return 0;
}
