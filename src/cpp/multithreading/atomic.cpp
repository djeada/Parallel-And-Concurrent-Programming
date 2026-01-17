/**
 * Atomic Operations
 *
 * This example demonstrates how std::atomic provides thread-safe operations
 * on shared data without requiring explicit locks. Atomic operations are
 * indivisible - they complete entirely without interruption.
 *
 * Key concepts:
 * - std::atomic<T> provides lock-free thread-safe operations for type T
 * - Operations like ++, --, +=, load(), store() are atomic
 * - Prevents data races when multiple threads access the same variable
 *
 * Compare this with simple_data_race.cpp to see the difference between
 * unprotected and atomic access to shared data.
 */

#include <atomic>
#include <iostream>
#include <thread>

// Atomic counter - all operations on this variable are thread-safe
std::atomic<unsigned int> task_count{0};

void worker() {
    for (int i = 0; i < 10'000'000; ++i) {
        // Atomic increment - guaranteed to be thread-safe
        ++task_count;
    }
}

int main() {
    std::thread alice(worker);
    std::thread bob(worker);

    alice.join();
    bob.join();

    // Expected result: exactly 20,000,000 (unlike simple_data_race.cpp)
    std::cout << "Completed " << task_count.load() << " tasks.\n";
    std::cout << "Expected:  20000000 tasks.\n";

    return 0;
}
