/**
 * Process Barrier
 *
 * This example demonstrates a barrier synchronization pattern
 * across multiple processes using a semaphore.
 *
 * Key concepts:
 * - Barrier ensures all processes reach a point before any proceed
 * - Useful for phased computation where stages must complete together
 * - Implementation using countdown and notification semaphores
 *
 * Note: This is a simplified barrier. For robust production use,
 * consider pthread_barrier_t (requires PTHREAD_PROCESS_SHARED) or
 * more sophisticated implementations.
 */

#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

void worker(int id, int* counter, sem_t* mutex, sem_t* barrier, int num_workers) {
    // Phase 1: Do initial work
    std::cout << "Worker " << id << " doing phase 1 work...\n";
    sleep(id % 3 + 1);  // Variable work time

    // Arrive at barrier
    sem_wait(mutex);
    (*counter)++;
    std::cout << "Worker " << id << " arrived at barrier ("
              << *counter << "/" << num_workers << ")\n";

    if (*counter == num_workers) {
        // Last to arrive - release all waiting
        for (int i = 0; i < num_workers; ++i) {
            sem_post(barrier);
        }
    }
    sem_post(mutex);

    // Wait at barrier
    sem_wait(barrier);

    // Phase 2: Continue after barrier
    std::cout << "Worker " << id << " passed barrier, doing phase 2\n";
}

int main() {
    constexpr int num_workers = 4;

    // Create shared counter
    int* counter = static_cast<int*>(
        mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE,
             MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    *counter = 0;

    // Create semaphores
    sem_unlink("/barrier_mutex");
    sem_unlink("/barrier_wait");
    sem_t* mutex = sem_open("/barrier_mutex", O_CREAT, 0644, 1);
    sem_t* barrier = sem_open("/barrier_wait", O_CREAT, 0644, 0);

    // Spawn workers
    for (int i = 0; i < num_workers; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            worker(i, counter, mutex, barrier, num_workers);
            _exit(0);
        }
    }

    // Wait for all workers
    for (int i = 0; i < num_workers; ++i) {
        wait(nullptr);
    }

    // Cleanup
    sem_close(mutex);
    sem_close(barrier);
    sem_unlink("/barrier_mutex");
    sem_unlink("/barrier_wait");
    munmap(counter, sizeof(int));

    std::cout << "All workers finished\n";
    return 0;
}
