/**
 * Multiple Child Processes
 *
 * This example demonstrates creating and managing multiple child
 * processes from a single parent.
 *
 * Key concepts:
 * - Multiple fork() calls create multiple independent processes
 * - Each child process has its own memory and execution context
 * - Parent must wait for ALL children to prevent zombie processes
 * - Children execute concurrently on multi-core systems
 *
 * This pattern is useful for parallelizing CPU-bound work across
 * separate processes with full memory isolation.
 */

#include <chrono>
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

void task_a() {
    std::cout << "Task A started (PID: " << getpid() << ")\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Task A finished\n";
}

void task_b() {
    std::cout << "Task B started (PID: " << getpid() << ")\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Task B finished\n";
}

int main() {
    std::cout << "Parent process (PID: " << getpid() << ")\n";

    std::vector<pid_t> children;

    // Create first child
    pid_t pid1 = fork();
    if (pid1 == 0) {
        task_a();
        return 0;
    }
    children.push_back(pid1);

    // Create second child
    pid_t pid2 = fork();
    if (pid2 == 0) {
        task_b();
        return 0;
    }
    children.push_back(pid2);

    // Parent waits for all children
    for (pid_t child : children) {
        int status;
        waitpid(child, &status, 0);
        std::cout << "Child " << child << " exited with status "
                  << WEXITSTATUS(status) << "\n";
    }

    std::cout << "All children finished\n";
    return 0;
}
