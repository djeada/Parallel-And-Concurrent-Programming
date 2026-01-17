/**
 * Deadlock Between Processes
 *
 * This example demonstrates a classic deadlock scenario between
 * processes using named semaphores.
 *
 * Key concepts:
 * - Deadlock occurs when processes wait for resources held by each other
 * - Four conditions: mutual exclusion, hold-and-wait, no preemption, circular wait
 * - Named semaphores (sem_open) work across processes
 * - This code WILL DEADLOCK to demonstrate the problem
 *
 * Solution strategies:
 * - Lock ordering: always acquire locks in the same order
 * - Try-lock with timeout
 * - Resource hierarchy
 *
 * WARNING: This program will hang indefinitely due to deadlock.
 */

#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

void process_a(sem_t* lock_a, sem_t* lock_b) {
    sem_wait(lock_a);
    std::cout << "Process A acquired lock_a\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Process A waiting for lock_b...\n";
    sem_wait(lock_b);  // Will block forever - B holds lock_b
    std::cout << "Process A acquired lock_b\n";

    sem_post(lock_b);
    sem_post(lock_a);
}

void process_b(sem_t* lock_a, sem_t* lock_b) {
    sem_wait(lock_b);
    std::cout << "Process B acquired lock_b\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Process B waiting for lock_a...\n";
    sem_wait(lock_a);  // Will block forever - A holds lock_a
    std::cout << "Process B acquired lock_a\n";

    sem_post(lock_a);
    sem_post(lock_b);
}

int main() {
    // Create named semaphores (shared between processes)
    sem_t* lock_a = sem_open("/deadlock_a", O_CREAT, 0644, 1);
    sem_t* lock_b = sem_open("/deadlock_b", O_CREAT, 0644, 1);

    pid_t pid = fork();

    if (pid == 0) {
        process_a(lock_a, lock_b);
    } else {
        process_b(lock_a, lock_b);
        wait(nullptr);
    }

    // Cleanup (never reached due to deadlock)
    sem_close(lock_a);
    sem_close(lock_b);
    sem_unlink("/deadlock_a");
    sem_unlink("/deadlock_b");

    return 0;
}
