/**
 * ⚠️  ANTIPATTERN — Process Deadlock (Circular Wait)
 *
 * Classic deadlock between two processes using named semaphores.
 *
 * Why it deadlocks — four necessary conditions:
 *   1. Mutual exclusion  — each semaphore is held by at most one process
 *   2. Hold-and-wait     — each holds one lock while waiting for the other
 *   3. No preemption     — locks cannot be forcibly taken away
 *   4. Circular wait     — A waits for B's lock; B waits for A's lock
 *
 * This example uses sem_timedwait() with a 2-second timeout so it exits
 * cleanly after demonstrating the deadlock instead of hanging forever.
 * The timeout also shows the correct cleanup pattern: always release
 * locks you already hold before exiting.
 *
 * See deadlock_avoidance.cpp for the fix (consistent lock ordering).
 *
 * Compile:
 *   g++ -std=c++17 -o deadlock deadlock.cpp -lpthread
 */

#include <cerrno>
#include <chrono>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

// Wait up to timeout_sec for sem; return true on success, false on timeout/error.
static bool timed_sem_wait(sem_t* sem, int timeout_sec) {
    struct timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_sec;
    while (true) {
        int rc = sem_timedwait(sem, &ts);
        if (rc == 0) return true;          // acquired
        if (errno == EINTR) continue;      // interrupted, retry
        return false;                      // ETIMEDOUT or other error
    }
}

void process_a(sem_t* lock_a, sem_t* lock_b) {
    // Acquire lock_a first (same order as process_b is WRONG here — B acquires B first)
    if (!timed_sem_wait(lock_a, 3)) {
        std::cout << "Process A: timeout waiting for lock_a\n";
        return;
    }
    std::cout << "Process A acquired lock_a\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Process A waiting for lock_b...\n";
    if (!timed_sem_wait(lock_b, 2)) {  // B holds lock_b → deadlock timeout
        std::cout << "Process A: ⚠️  DEADLOCK DETECTED (timeout waiting for lock_b)\n";
        std::cout << "Process A: releasing lock_a before exit\n";
        sem_post(lock_a);  // always release held locks before exiting
        return;
    }
    std::cout << "Process A acquired lock_b\n";
    sem_post(lock_b);
    sem_post(lock_a);
}

void process_b(sem_t* lock_a, sem_t* lock_b) {
    // Acquire lock_b first — opposite order from A → circular wait
    if (!timed_sem_wait(lock_b, 3)) {
        std::cout << "Process B: timeout waiting for lock_b\n";
        return;
    }
    std::cout << "Process B acquired lock_b\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Process B waiting for lock_a...\n";
    if (!timed_sem_wait(lock_a, 2)) {  // A holds lock_a → deadlock timeout
        std::cout << "Process B: ⚠️  DEADLOCK DETECTED (timeout waiting for lock_a)\n";
        std::cout << "Process B: releasing lock_b before exit\n";
        sem_post(lock_b);
        return;
    }
    std::cout << "Process B acquired lock_a\n";
    sem_post(lock_a);
    sem_post(lock_b);
}

int main() {
    std::cout << "=== Deadlock demo (sem_timedwait with 2s timeout) ===\n\n";

    // Remove stale semaphores from previous interrupted runs
    sem_unlink("/deadlock_a");
    sem_unlink("/deadlock_b");

    sem_t* lock_a = sem_open("/deadlock_a", O_CREAT | O_EXCL, 0644, 1);
    sem_t* lock_b = sem_open("/deadlock_b", O_CREAT | O_EXCL, 0644, 1);
    if (lock_a == SEM_FAILED || lock_b == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        process_a(lock_a, lock_b);
        _exit(0);
    } else {
        process_b(lock_a, lock_b);
        wait(nullptr);
    }

    std::cout << "\nBoth processes exited. Note: no work was done — deadlock.\n";
    std::cout << "See deadlock_avoidance.cpp for the fix.\n";

    sem_close(lock_a);
    sem_close(lock_b);
    sem_unlink("/deadlock_a");
    sem_unlink("/deadlock_b");

    return 0;
}
