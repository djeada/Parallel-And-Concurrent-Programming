/**
 * Thread Pool
 *
 * This example demonstrates a thread pool - a collection of worker threads
 * that efficiently execute queued tasks.
 *
 * Key concepts:
 * - Fixed number of threads avoids thread creation overhead
 * - Tasks are queued and executed by available workers
 * - Workers wait on condition_variable when queue is empty
 * - Destructor signals workers to stop and waits for completion
 *
 * Thread pools are essential for managing concurrent workloads efficiently.
 */

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) : stop_(false) {
        workers_.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back(&ThreadPool::worker_loop, this);
        }
    }

    ~ThreadPool() {
        {
            std::scoped_lock lock(queue_mutex_);
            stop_ = true;
        }
        cv_.notify_all();

        for (auto& worker : workers_) {
            worker.join();
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
    void worker_loop() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                cv_.wait(lock, [this] {
                    return stop_ || !tasks_.empty();
                });

                if (stop_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
            }

            task();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool stop_;
};

void process_task(int id) {
    std::cout << "Thread " << std::this_thread::get_id()
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

    std::cout << "All tasks completed.\n";
    return 0;
}
