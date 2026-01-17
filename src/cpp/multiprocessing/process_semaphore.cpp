/**
 * Process Semaphore
 *
 * This example demonstrates using named semaphores to limit
 * concurrent access to a shared resource across processes.
 *
 * Key concepts:
 * - Named semaphores (sem_open) work across unrelated processes
 * - Semaphore count limits concurrent access
 * - sem_wait() decrements (blocks if zero), sem_post() increments
 * - Must unlink semaphore when done to clean up
 *
 * Unlike a mutex (binary semaphore), counting semaphores allow
 * multiple concurrent accessors up to the limit.
 */

#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <random>
#include <semaphore.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

constexpr char SEM_NAME[] = "/resource_semaphore";
constexpr int MAX_CONCURRENT = 3;

void access_resource(sem_t* sem, int id) {
    std::cout << "Process " << id << " waiting for resource...\n";

    sem_wait(sem);
    std::cout << "Process " << id << " acquired resource\n";

    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Process " << id << " releasing resource\n";
    sem_post(sem);
}

int main() {
    constexpr int num_processes = 10;

    // Create semaphore with count = MAX_CONCURRENT
    sem_unlink(SEM_NAME);  // Clean up any existing
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0644, MAX_CONCURRENT);

    std::vector<pid_t> children;

    for (int i = 0; i < num_processes; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            access_resource(sem, i);
            _exit(0);
        }
        children.push_back(pid);
    }

    // Wait for all children
    for (pid_t pid : children) {
        waitpid(pid, nullptr, 0);
    }

    sem_close(sem);
    sem_unlink(SEM_NAME);

    std::cout << "All processes completed\n";
    return 0;
}
