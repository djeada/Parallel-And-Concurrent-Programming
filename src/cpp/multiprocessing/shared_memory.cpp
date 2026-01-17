/**
 * Shared Memory Between Processes
 *
 * This example demonstrates sharing memory between processes using
 * POSIX shared memory (shm_open + mmap).
 *
 * Key concepts:
 * - shm_open() creates a named shared memory object
 * - mmap() maps it into the process's address space
 * - Semaphores protect concurrent access to shared data
 * - Changes in one process are visible to all others
 * - Must manually synchronize access (unlike threads, no atomic types)
 *
 * Shared memory is the fastest IPC mechanism but requires
 * careful synchronization to prevent data races.
 */

#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

constexpr char SHM_NAME[] = "/example_shm";
constexpr char SEM_NAME[] = "/example_sem";

void incrementer(int* shared_value, sem_t* sem) {
    for (int i = 0; i < 5; ++i) {
        sem_wait(sem);
        ++(*shared_value);
        std::cout << "Incrementer: " << *shared_value << "\n";
        sem_post(sem);
        usleep(100'000);
    }
}

void decrementer(int* shared_value, sem_t* sem) {
    for (int i = 0; i < 5; ++i) {
        sem_wait(sem);
        --(*shared_value);
        std::cout << "Decrementer: " << *shared_value << "\n";
        sem_post(sem);
        usleep(100'000);
    }
}

int main() {
    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    ftruncate(shm_fd, sizeof(int));

    int* shared_value = static_cast<int*>(
        mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE,
             MAP_SHARED, shm_fd, 0));
    if (shared_value == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    *shared_value = 0;

    // Create semaphore for synchronization
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        incrementer(shared_value, sem);
        _exit(0);
    } else {
        // Parent process
        decrementer(shared_value, sem);
        waitpid(pid, nullptr, 0);

        std::cout << "Final value: " << *shared_value << "\n";

        // Cleanup
        munmap(shared_value, sizeof(int));
        close(shm_fd);
        sem_close(sem);
        shm_unlink(SHM_NAME);
        sem_unlink(SEM_NAME);
    }

    return 0;
}
