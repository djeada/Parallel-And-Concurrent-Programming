/**
 * std::packaged_task and Callbacks
 *
 * This example demonstrates std::packaged_task, which wraps a callable
 * and provides a future for its result.
 *
 * Key concepts:
 * - std::packaged_task wraps any callable (function, lambda, functor)
 * - get_future() returns a future associated with the task
 * - The task can be moved to a thread and executed there
 * - Results are retrieved via the future after thread completion
 *
 * packaged_task is useful when you need more control over task
 * scheduling than std::async provides, or when implementing
 * custom thread pools.
 */

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int slow_square(int x) {
    std::cout << "Computing square of " << x << "...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Done with " << x << "\n";
    return x * x;
}

void process_result(std::future<int>& future) {
    std::cout << "Result: " << future.get() << "\n";
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    // Create packaged tasks
    std::packaged_task<int(int)> task1(slow_square);
    std::packaged_task<int(int)> task2(slow_square);

    // Get futures before moving tasks
    std::future<int> future1 = task1.get_future();
    std::future<int> future2 = task2.get_future();

    // Execute tasks in threads
    std::thread t1(std::move(task1), 3);
    std::thread t2(std::move(task2), 4);

    // Wait for threads to complete
    t1.join();
    t2.join();

    // Process results via futures
    process_result(future1);
    process_result(future2);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Total time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
              << " ms\n";

    return 0;
}
