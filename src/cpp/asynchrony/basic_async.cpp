/**
 * Basic std::async Usage
 *
 * This example demonstrates the fundamental difference between
 * synchronous and asynchronous execution using std::async.
 *
 * Key concepts:
 * - std::async launches a function asynchronously (potentially in another thread)
 * - std::launch::async forces execution in a new thread
 * - std::future represents the result of an async operation
 * - wait() blocks until the async operation completes
 *
 * Asynchronous execution allows tasks to run concurrently, reducing
 * total execution time when tasks are independent.
 */

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

void task(int id, int sleep_seconds) {
    std::cout << "Task " << id << " started\n";
    std::this_thread::sleep_for(std::chrono::seconds(sleep_seconds));
    std::cout << "Task " << id << " finished\n";
}

void run_synchronous() {
    auto start = std::chrono::steady_clock::now();

    task(1, 2);
    task(2, 1);

    auto elapsed = std::chrono::steady_clock::now() - start;
    std::cout << "Synchronous: "
              << std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()
              << " seconds (expected: 3)\n\n";
}

void run_asynchronous() {
    auto start = std::chrono::steady_clock::now();

    // Launch tasks asynchronously - they run concurrently
    auto future1 = std::async(std::launch::async, task, 1, 2);
    auto future2 = std::async(std::launch::async, task, 2, 1);

    future1.wait();
    future2.wait();

    auto elapsed = std::chrono::steady_clock::now() - start;
    std::cout << "Asynchronous: "
              << std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()
              << " seconds (expected: 2)\n";
}

int main() {
    run_synchronous();
    run_asynchronous();
    return 0;
}
