/**
 * Modern C++20: Thread Pool with Owned Workers
 *
 * This example demonstrates a thread pool - a collection of worker threads
 * that efficiently execute queued tasks.
 *
 * Key concepts:
 * - Fixed number of threads avoids thread creation overhead
 * - Tasks are queued and executed by available workers
 * - Workers use stop-aware condition_variable_any waits
 * - Destructor requests stop, drains queued work, and joins via std::jthread
 * - Worker members are destroyed before the state they access
 * - Submitted callables must not throw; use asynchrony/thread_pool_futures.cpp
 *   for a result/exception channel (path relative to src/cpp)
 *
 * Thread pools are essential for managing concurrent workloads efficiently.
 */

#include <condition_variable>
#include <functional>
#include <iostream>
#include <syncstream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) {
        if (num_threads == 0) {
            throw std::invalid_argument("ThreadPool needs at least one worker");
        }
        workers_.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this](std::stop_token stop) { worker_loop(stop); });
        }
    }

    ~ThreadPool() {
        // Stop-aware waits wake automatically. Ask every worker to stop before
        // vector destruction joins them, keeping queue state alive until then.
        for (auto& worker : workers_) {
            worker.request_stop();
        }
    }

    // Prevent copying
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void enqueue(std::function<void()> task) {
        {
            std::scoped_lock lock(queue_mutex_);
            tasks_.push(std::move(task));
        }
        cv_.notify_one();
    }

private:
    void worker_loop(std::stop_token stop) {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                cv_.wait(lock, stop, [this] { return !tasks_.empty(); });

                if (tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            task();
        }
    }

    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable_any cv_;
    std::vector<std::jthread> workers_; // Destroy first, including on constructor failure.
};

void process_task(int id) {
    std::osyncstream(std::cout) << "Thread " << std::this_thread::get_id()
              << " processed task " << id << ".\n";
}

int main() {
    constexpr int num_threads = 4;
    constexpr int num_tasks = 20;

    {
        ThreadPool pool(num_threads);

        for (int i = 0; i < num_tasks; ++i) {
            pool.enqueue([i] { process_task(i); });
        }

        // Pool destructor waits for all tasks to complete
    }

    std::osyncstream(std::cout) << "All tasks completed.\n";
    return 0;
}
