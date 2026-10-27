/**
 * Modern C++20 Thread Pool with std::packaged_task and std::jthread
 *
 * std::async is convenient but gives you no control over:
 *   - how many threads run concurrently
 *   - thread reuse (it may spawn a new thread per task)
 *   - task queuing / ordering
 *
 * A thread pool solves all three:
 *   - Fixed number of worker threads
 *   - Tasks queued and picked up by idle workers
 *   - Each submitted task returns a std::future for the result
 *
 * This implementation uses std::packaged_task to bridge a callable
 * into a future, and a queue + stop-aware condition_variable_any for dispatch.
 * Shutdown drains accepted jobs before automatically joining owned workers.
 *
 * Compile:
 *   g++ -std=c++20 -pthread -O1 thread_pool_futures.cpp
 */

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t num_threads) {
        if (num_threads == 0) {
            throw std::invalid_argument("ThreadPool needs at least one worker");
        }
        workers_.reserve(num_threads);
        for (std::size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this](std::stop_token stop) { worker_loop(stop); });
        }
    }

    ~ThreadPool() {
        for (auto& worker : workers_) {
            worker.request_stop();
        }
        // The workers_ member is destroyed first and joins while state is alive.
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Submit a callable and return a future for its result.
    // The owner must keep the pool alive throughout submission.
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            [func = std::forward<F>(f),
             targs = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                return std::apply(std::move(func), std::move(targs));
            });

        std::future<ReturnType> fut = task->get_future();

        {
            std::scoped_lock lock(mutex_);
            queue_.push([task]() { (*task)(); });
        }
        cv_.notify_one();

        return fut;
    }

    std::size_t thread_count() const { return workers_.size(); }

private:
    void worker_loop(std::stop_token stop) {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, stop, [this] { return !queue_.empty(); });
                // A stop request wakes idle workers; keep draining queued work.
                if (queue_.empty()) return;
                job = std::move(queue_.front());
                queue_.pop();
            }
            job();
        }
    }

    std::queue<std::function<void()>> queue_;
    std::mutex                        mutex_;
    std::condition_variable_any       cv_;
    std::vector<std::jthread>         workers_; // Destroy before queue/mutex/cv.
};

// ── Demo ───────────────────────────────────────────────────────────────────

int square(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return x * x;
}

int main() {
    std::cout << "=== Thread pool: " << std::thread::hardware_concurrency()
              << " hardware threads, pool size = 4 ===\n\n";

    ThreadPool pool(4);

    // Submit 10 tasks — at most 4 run at once
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.submit(square, i));
    }

    std::cout << "Results:\n";
    for (int i = 0; i < (int)futures.size(); ++i) {
        std::cout << "  " << i << "^2 = " << futures[i].get() << "\n";
    }

    // ── Exception propagation through the pool ─────────────────────────
    std::cout << "\nException propagation demo:\n";
    auto bad_fut = pool.submit([]() -> int {
        throw std::runtime_error("task failed");
        return 0;
    });

    try {
        bad_fut.get();
    } catch (const std::exception& e) {
        std::cout << "  caught from pool task: " << e.what() << "\n";
    }

    // Scope exit requests stop, drains jobs, and joins all workers.
    std::cout << "\nAll results retrieved; leaving pool scope.\n";
    return 0;
}
