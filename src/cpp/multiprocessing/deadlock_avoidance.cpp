/**
 * Deadlock Avoidance — Lock Ordering
 *
 * This is the FIX for deadlock.cpp.
 *
 * Root cause of the deadlock in deadlock.cpp:
 *   Process A acquires lock_a then tries lock_b.
 *   Process B acquires lock_b then tries lock_a.
 *   → Circular wait: neither can proceed.
 *
 * Solution — consistent lock ordering:
 *   All processes must acquire locks in the SAME order.
 *   Here, both processes always acquire lock_a before lock_b.
 *   Circular wait is structurally impossible.
 *
 * This is the simplest and most effective deadlock prevention technique.
 * Other strategies (lock hierarchies, try-lock with back-off, lock
 * timeout, and deadlock detection) are more complex but useful when
 * a global ordering is impractical.
 *
 * Compile:
 *   g++ -std=c++17 -o deadlock_avoidance deadlock_avoidance.cpp -lpthread
 */

#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

static sem_t* open_sem(const char* name) {
    sem_unlink(name);  // remove stale semaphore from previous runs
    sem_t* s = sem_open(name, O_CREAT | O_EXCL, 0644, 1);
    if (s == SEM_FAILED) { perror("sem_open"); _exit(1); }
    return s;
}

// Both processes acquire in the SAME order: lock_a → lock_b
static void process_fn(const char* name, sem_t* lock_a, sem_t* lock_b) {
    std::cout << name << ": acquiring lock_a...\n";
    sem_wait(lock_a);
    std::cout << name << ": acquired lock_a\n";

    // Simulate some work while holding lock_a
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << name << ": acquiring lock_b...\n";
    sem_wait(lock_b);
    std::cout << name << ": acquired lock_b — doing work\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    sem_post(lock_b);
    std::cout << name << ": released lock_b\n";
    sem_post(lock_a);
    std::cout << name << ": released lock_a — done\n";
}

int main() {
    std::cout << "=== Deadlock avoidance via lock ordering ===\n\n";
    std::cout << "Both processes acquire lock_a first, then lock_b.\n";
    std::cout << "Circular wait is structurally impossible.\n\n";

    sem_t* lock_a = open_sem("/avoid_lock_a");
    sem_t* lock_b = open_sem("/avoid_lock_b");

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        process_fn("Process A", lock_a, lock_b);
        _exit(0);
    } else {
        process_fn("Process B", lock_a, lock_b);
        wait(nullptr);
    }

    std::cout << "\nBoth processes finished successfully — no deadlock.\n";

    sem_close(lock_a);
    sem_close(lock_b);
    sem_unlink("/avoid_lock_a");
    sem_unlink("/avoid_lock_b");

    return 0;
}
