/**
 * This script demonstrates the use of shared memory and semaphores in C++ to share a value
 * between multiple processes using the POSIX standard library.
 *
 * In this example, we create two processes: an incrementer and a decrementer. The incrementer increases
 * the shared value by 1, while the decrementer decreases the shared value by 1. We use a semaphore to ensure
 * that the operations on the shared value are synchronized and prevent race conditions.
 */

#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
constexpr char SHM_NAME[] = "/shared_memory";
constexpr char SEM_NAME[] = "/shared_semaphore";

void incrementer() {
    // Open shared memory and semaphore
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    sem_t *sem = sem_open(SEM_NAME, 0);

    // Map shared memory to the address space
    int *shared_value = static_cast<int *>(mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    // Perform increments
    for (int i = 0; i < 10; ++i) {
        sem_wait(sem);
        ++(*shared_value);
        std::cout << "Incrementer: " << *shared_value << std::endl;
        sem_post(sem);

        sleep(1);
    }

    // Clean up
    munmap(shared_value, sizeof(int));
    close(shm_fd);
    sem_close(sem);
}

void decrementer() {
    // Open shared memory and semaphore
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    sem_t *sem = sem_open(SEM_NAME, 0);

    // Map shared memory to the address space
    int *shared_value = static_cast<int *>(mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    // Perform decrements
    for (int i = 0; i < 10; ++i) {
        sem_wait(sem);
        --(*shared_value);
        std::cout << "Decrementer: " << *shared_value << std::endl;
        sem_post(sem);

        sleep(1);
    }

    // Clean up
    munmap(shared_value, sizeof(int));
    close(shm_fd);
    sem_close(sem);
}

int main() {
    // Create shared memory and semaphore
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(int));
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    // Fork a new process
    pid_t pid = fork();

    if (pid == 0) {
        incrementer();
    } else {
        decrementer();

        // Wait for the child process to finish
        waitpid(pid, nullptr, 0);

        // Clean up
        sem_close(sem);
        sem_unlink(SEM_NAME);
        shm_unlink(SHM_NAME);
    }

    return 0;
}
