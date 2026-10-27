/**
 * Modern C++20: Standard Semaphore with Future-Based Tasks
 *
 * This example demonstrates using a counting semaphore to limit
 * concurrent access to a resource from async tasks.
 *
 * Key concepts:
 * - Semaphore limits concurrent access to N tasks
 * - acquire() blocks if count is 0
 * - release() increments count and wakes waiting tasks
 * - Limits concurrent access; this is not a requests-per-second rate limiter
 *
 * acquire() blocks the calling worker thread; it does not suspend a coroutine.
 * Compare ../multithreading/legacy/semaphore.cpp for the manual implementation.
 * Compile: g++ -std=c++20 -pthread async_semaphore.cpp
 */

#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#include <semaphore>
#include <syncstream>
#include <thread>
#include <vector>

// Adapt the semaphore to the lock/unlock interface so lock_guard restores the
// permit even when work throws. std::counting_semaphore itself has no lock().
class ResourceLimit {
public:
    void lock() { permits_.acquire(); }
    void unlock() { permits_.release(); }

private:
    std::counting_semaphore<3> permits_{3};
};

ResourceLimit resource_limit;  // Only 3 concurrent accessors

void access_resource(int task_id) {
    std::lock_guard permit(resource_limit);

    std::osyncstream(std::cout) << "Task " << task_id << " using resource\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(500, 2000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));

    std::osyncstream(std::cout) << "Task " << task_id << " done\n";
}

int main() {
    std::vector<std::future<void>> tasks;

    for (int i = 0; i < 10; ++i) {
        tasks.push_back(std::async(std::launch::async, access_resource, i));
    }

    for (auto& t : tasks) {
        t.get();
    }

    std::cout << "All tasks completed\n";
    return 0;
}
