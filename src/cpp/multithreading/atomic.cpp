/**
 * Atomic Operations
 *
 * This example demonstrates how std::atomic provides thread-safe operations
 * on shared data without requiring explicit locks. Each atomic update is
 * indivisible with respect to competing accesses to that atomic object.
 *
 * Key concepts:
 * - std::atomic<T> provides atomic operations; lock freedom depends on T/platform
 * - std::jthread joins each worker automatically at the end of its scope
 * - Operations like ++, --, +=, load(), store() are atomic
 * - Prevents data races when multiple threads access the same variable
 *
 * Compare this with simple_data_race_antipattern.cpp to see the difference between
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
        // This counter publishes no other data; joining waits for completion.
        task_count.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    {
        std::jthread alice(worker);
        std::jthread bob(worker);
    }

    // Expected result: exactly 20,000,000 (unlike simple_data_race_antipattern.cpp)
    std::cout << "Completed " << task_count.load() << " tasks.\n";
    std::cout << "Expected:  20000000 tasks.\n";

    return 0;
}
