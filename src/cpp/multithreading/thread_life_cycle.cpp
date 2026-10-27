/**
 * Modern C++20: Worker Lifetime and Cooperative Cancellation
 *
 * A jthread passes its stop_token to the worker. request_stop() asks the
 * worker to return; destruction joins it. Cancellation is cooperative:
 * blocking operations must support stopping, or have a bounded wait.
 *
 * Compile: g++ -std=c++20 -pthread thread_life_cycle.cpp
 */

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

void task_worker(std::stop_token stop, const char* name) {
    unsigned int task_count = 0;
    std::mutex mutex;
    std::condition_variable_any wake;
    std::unique_lock lock(mutex);

    while (!stop.stop_requested()) {
        ++task_count;
        // Simulate waiting between jobs. A stop request interrupts this wait.
        wake.wait_for(lock, stop, 100ms, [] { return false; });
    }

    std::osyncstream(std::cout) << name << " completed " << task_count << " tasks.\n";
}

int main() {
    {
        std::jthread alice(task_worker, "Alice");
        std::jthread bob(task_worker, "Bob");
        std::cout << "Workers are running...\n";
        std::this_thread::sleep_for(1s);

        // Ask both workers to stop before either destructor waits for them.
        alice.request_stop();
        bob.request_stop();
    }

    std::cout << "All workers stopped.\n";
}
