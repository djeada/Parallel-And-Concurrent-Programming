/**
 * Process Mutex (Inter-Process Lock)
 *
 * This example demonstrates using a POSIX mutex shared between
 * processes to protect a critical section.
 *
 * Key concepts:
 * - PTHREAD_PROCESS_SHARED attribute allows mutex sharing across processes
 * - Mutex must be in shared memory (mmap with MAP_SHARED)
 * - Provides mutual exclusion just like thread mutexes
 * - Essential for protecting shared data from race conditions
 *
 * Without the mutex, concurrent increments would produce incorrect results
 * due to the read-modify-write race condition.
 */

#include <iostream>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

struct SharedData {
    pthread_mutex_t mutex;
    int counter;
};

void worker(SharedData* data, int id) {
    for (int i = 0; i < 1000; ++i) {
        pthread_mutex_lock(&data->mutex);
        ++data->counter;
        pthread_mutex_unlock(&data->mutex);
    }
    std::cout << "Worker " << id << " finished\n";
}

int main() {
    constexpr int num_workers = 4;

    // Create shared memory for data and mutex
    SharedData* data = static_cast<SharedData*>(
        mmap(nullptr, sizeof(SharedData),
             PROT_READ | PROT_WRITE,
             MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    // Initialize mutex with PTHREAD_PROCESS_SHARED
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
    data->counter = 0;

    // Spawn worker processes
    for (int i = 0; i < num_workers; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            worker(data, i);
            _exit(0);
        }
    }

    // Wait for all workers
    for (int i = 0; i < num_workers; ++i) {
        wait(nullptr);
    }

    std::cout << "Final counter: " << data->counter << "\n";
    std::cout << "Expected: " << (num_workers * 1000) << "\n";

    // Cleanup
    pthread_mutex_destroy(&data->mutex);
    pthread_mutexattr_destroy(&attr);
    munmap(data, sizeof(SharedData));

    return 0;
}
