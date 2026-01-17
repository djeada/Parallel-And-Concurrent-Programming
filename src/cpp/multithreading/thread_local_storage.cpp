/**
 * Thread Local Storage
 *
 * This example demonstrates thread_local storage - variables that have
 * a separate instance for each thread.
 *
 * Key concepts:
 * - thread_local variables have per-thread storage
 * - Each thread gets its own copy, initialized independently
 * - No synchronization needed - each thread owns its copy
 * - Useful for caching, error state, or thread-specific context
 *
 * Without thread_local, these variables would need mutex protection.
 */

#include <iostream>
#include <thread>
#include <vector>

// Each thread gets its own copy of this variable
thread_local int thread_data = 0;

void do_work() {
    thread_data *= 2;  // Safe - only this thread accesses this copy
}

void worker(int id) {
    // Initialize this thread's copy
    thread_data = id;

    // Perform work that modifies thread-local data
    do_work();

    // Each thread sees its own value
    std::cout << "Thread " << id << " has value: " << thread_data << "\n";
}

int main() {
    constexpr int num_threads = 5;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int i = 1; i <= num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    // Each thread sees: thread_id * 2
    // Thread 1 -> 2, Thread 2 -> 4, Thread 3 -> 6, etc.
    std::cout << "All threads complete.\n";

    return 0;
}
