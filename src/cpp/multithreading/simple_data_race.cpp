/**
 * Data Race (Unsafe Code)
 *
 * This example demonstrates a DATA RACE - undefined behavior that occurs
 * when multiple threads access the same memory location concurrently,
 * and at least one access is a write.
 *
 * Key concepts:
 * - Data races lead to undefined behavior (UB)
 * - The final value is unpredictable (not 20,000,000)
 * - Reading and writing shared data without synchronization is unsafe
 * - Compare with atomic.cpp and simple_mutex.cpp for correct solutions
 *
 * WARNING: This code intentionally contains a bug for educational purposes!
 */

#include <iostream>
#include <thread>

// UNSAFE: This variable is accessed by multiple threads without protection
unsigned int task_count = 0;

void worker() {
    for (int i = 0; i < 10'000'000; ++i) {
        // DATA RACE: Multiple threads reading and writing without sync
        ++task_count;  // This is NOT atomic!
    }
}

int main() {
    std::thread alice(worker);
    std::thread bob(worker);

    alice.join();
    bob.join();

    // Result is UNPREDICTABLE due to data race
    std::cout << "Completed " << task_count << " tasks.\n";
    std::cout << "Expected:  20000000 tasks.\n";
    std::cout << "(Result varies due to data race!)\n";

    return 0;
}
