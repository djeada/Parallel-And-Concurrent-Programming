/**
 * Semaphore with Async Tasks
 *
 * This example demonstrates using a counting semaphore to limit
 * concurrent access to a resource from async tasks.
 *
 * Key concepts:
 * - Semaphore limits concurrent access to N tasks
 * - acquire() blocks if count is 0
 * - release() increments count and wakes waiting tasks
 * - Useful for rate limiting or resource pooling
 *
 * Note: C++20 provides std::counting_semaphore, but this shows
 * the underlying mechanism using mutex and condition_variable.
 */

#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

class Semaphore {
public:
    explicit Semaphore(int count) : count_(count) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return count_ > 0; });
        --count_;
    }

    void release() {
        std::scoped_lock lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};

Semaphore resource_limit(3);  // Only 3 concurrent accessors

void access_resource(int task_id) {
    resource_limit.acquire();

    std::cout << "Task " << task_id << " using resource\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(500, 2000);
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));

    std::cout << "Task " << task_id << " done\n";

    resource_limit.release();
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
