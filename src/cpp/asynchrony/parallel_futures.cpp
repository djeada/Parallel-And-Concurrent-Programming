/**
 * Parallel Futures — Multiple Concurrent std::async Tasks
 *
 * Demonstrates launching several independent computations in parallel
 * with std::async and collecting results through std::future.
 *
 * Key concepts:
 * - std::async returns a std::future<T> where T is the return type
 * - Launch multiple futures before calling .get() to run them in parallel
 * - future.get() blocks until result is available and returns it
 * - get() can only be called once per future (state is consumed)
 * - Total wall-clock time ≈ max(individual task times), not their sum
 *
 * Compare with future.cpp (single task) and packaged_task.cpp (explicit thread).
 * See also async_antipatterns.cpp for pitfalls to avoid.
 *
 * Compile:
 *   g++ -std=c++17 -pthread -O1 parallel_futures.cpp
 */

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int slow_square(int x) {
    std::cout << "Computing square of " << x << "...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Done computing square of " << x << "\n";
    return x * x;
}

void run_synchronous() {
    auto start = std::chrono::steady_clock::now();

    int result1 = slow_square(3);
    int result2 = slow_square(4);

    auto elapsed = std::chrono::steady_clock::now() - start;
    std::cout << "Synchronous: " << result1 << ", " << result2
              << " in " << std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()
              << "s\n\n";
}

void run_asynchronous() {
    auto start = std::chrono::steady_clock::now();

    // Launch computations in parallel
    auto future1 = std::async(std::launch::async, slow_square, 3);
    auto future2 = std::async(std::launch::async, slow_square, 4);

    // Retrieve results (blocks until ready)
    int result1 = future1.get();
    int result2 = future2.get();

    auto elapsed = std::chrono::steady_clock::now() - start;
    std::cout << "Asynchronous: " << result1 << ", " << result2
              << " in " << std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()
              << "s\n";
}

int main() {
    run_synchronous();
    run_asynchronous();
    return 0;
}
