/**
 * Simple Future Example
 *
 * This example demonstrates the basic usage of std::future
 * to retrieve a result from an async task.
 *
 * Key concepts:
 * - std::async launches work in a background thread
 * - std::future holds the eventual result
 * - Main thread can continue while work progresses
 * - future.get() blocks until result is ready
 *
 * This is the simplest pattern for async result retrieval.
 */

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

int process_task() {
    std::cout << "Worker processing...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 42;
}

int main() {
    std::cout << "Starting async task\n";

    std::future<int> result = std::async(std::launch::async, process_task);

    std::cout << "Main thread can do other work...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Waiting for result...\n";

    int value = result.get();  // Blocks until ready
    std::cout << "Result: " << value << "\n";

    return 0;
}
