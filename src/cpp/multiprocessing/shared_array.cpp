/**
 * ⚠️  ANTIPATTERN — Shared Array Without Synchronisation
 *
 * Demonstrates anonymous shared memory (mmap MAP_ANONYMOUS) and the
 * data races that occur when multiple processes modify it concurrently
 * without any locking.
 *
 * MAP_ANONYMOUS concepts (the good part):
 *   - mmap with MAP_SHARED | MAP_ANONYMOUS creates a shared region
 *     visible to all processes that fork from the creator.
 *   - Simpler than shm_open when only parent/child access is needed.
 *   - No filesystem name; region is automatically cleaned up on exit.
 *
 * The race condition (the bad part):
 *   - ++arr[i] is not atomic: it compiles to load → increment → store.
 *   - Two processes can each load the same value, both increment, and
 *     one of the stores is lost — final value is unpredictable.
 *   - The final array values may be wrong and vary between runs.
 *
 * Fix: use a pthread_mutex_t or sem_t in shared memory to protect each
 * array element, or use a PTHREAD_PROCESS_SHARED rwlock.
 * See shared_memory.cpp for the synchronized version with a semaphore.
 *
 * Compile:
 *   g++ -std=c++17 -o shared_array shared_array.cpp
 */

#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

constexpr size_t ARRAY_SIZE = 5;

void increment_array(int* arr, int iterations) {
    for (int n = 0; n < iterations; ++n) {
        for (size_t i = 0; i < ARRAY_SIZE; ++i) {
            ++arr[i];
        }
        usleep(100'000);
    }
}

void decrement_array(int* arr, int iterations) {
    for (int n = 0; n < iterations; ++n) {
        for (size_t i = 0; i < ARRAY_SIZE; ++i) {
            --arr[i];
        }
        usleep(100'000);
    }
}

void print_array(const char* label, int* arr) {
    std::cout << label << ": [";
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        std::cout << arr[i];
        if (i < ARRAY_SIZE - 1) std::cout << ", ";
    }
    std::cout << "]\n";
}

int main() {
    // Create anonymous shared memory for the array
    int* shared_array = static_cast<int*>(
        mmap(nullptr, sizeof(int) * ARRAY_SIZE,
             PROT_READ | PROT_WRITE,
             MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    // Initialize array
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        shared_array[i] = 0;
    }

    print_array("Initial", shared_array);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        increment_array(shared_array, 3);
        print_array("After increment", shared_array);
        _exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        decrement_array(shared_array, 3);
        print_array("After decrement", shared_array);
        _exit(0);
    }

    // Wait for children
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    print_array("Final", shared_array);

    munmap(shared_array, sizeof(int) * ARRAY_SIZE);
    return 0;
}
