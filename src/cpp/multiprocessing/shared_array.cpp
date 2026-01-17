/**
 * Shared Array Between Processes
 *
 * This example demonstrates sharing an array between processes
 * using anonymous shared memory (mmap with MAP_ANONYMOUS).
 *
 * Key concepts:
 * - MAP_ANONYMOUS creates shared memory without a file
 * - MAP_SHARED makes memory visible to child processes after fork()
 * - Array elements can be modified by any process with access
 * - Without synchronization, race conditions can occur
 *
 * Anonymous shared memory is useful when:
 * - Only related processes (parent/child) need access
 * - You don't need persistence across program runs
 * - Cleanup happens automatically when all processes exit
 *
 * WARNING: This example has intentional race conditions for demonstration.
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
