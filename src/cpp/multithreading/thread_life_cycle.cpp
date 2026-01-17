/**
 * Thread Life Cycle
 *
 * This example demonstrates the lifecycle of a thread and how to
 * communicate between threads using atomic variables.
 *
 * Key concepts:
 * - Thread starts executing immediately after construction
 * - std::atomic<bool> provides thread-safe flag communication
 * - Threads run until their function returns
 * - join() waits for thread completion
 *
 * The pattern shown here allows the main thread to signal workers
 * to stop after a certain time has elapsed.
 */

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

std::atomic<bool> running{true};

void task_worker(const char* name) {
    unsigned int task_count = 0;

    // Run until signaled to stop
    while (running) {
        ++task_count;
    }

    std::cout << name << " completed " << task_count << " tasks.\n";
}

int main() {
    std::thread worker1(task_worker, "Alice");
    std::thread worker2(task_worker, "Bob");

    std::cout << "Workers are running...\n";

    // Let workers run for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Signal workers to stop
    running = false;

    // Wait for workers to finish
    worker1.join();
    worker2.join();

    std::cout << "All workers stopped.\n";
    return 0;
}
