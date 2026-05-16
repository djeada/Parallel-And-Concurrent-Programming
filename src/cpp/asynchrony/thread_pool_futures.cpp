/**
 * Thread Pool with std::packaged_task — Future-Based Interface
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
 * into a future, and a std::queue + condition_variable for work dispatch.
 *
 * Compile:
 *   g++ -std=c++17 -pthread -O1 thread_pool_futures.cpp
 */

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t num_threads) {
        for (std::size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~ThreadPool() {
        {
            std::scoped_lock lock(mutex_);
            shutdown_ = true;
        }
        cv_.notify_all();
        for (auto& t : workers_) t.join();
    }

    // Submit a callable and return a future for its result.
    // Throws std::runtime_error if the pool has been shut down.
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
            if (shutdown_)
                throw std::runtime_error("submit() called on a stopped ThreadPool");
            queue_.push([task]() { (*task)(); });
        }
        cv_.notify_one();

        return fut;
    }

    std::size_t thread_count() const { return workers_.size(); }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return shutdown_ || !queue_.empty(); });
                if (shutdown_ && queue_.empty()) return;
                job = std::move(queue_.front());
                queue_.pop();
            }
            job();
        }
    }

    std::vector<std::thread>          workers_;
    std::queue<std::function<void()>> queue_;
    std::mutex                        mutex_;
    std::condition_variable           cv_;
    bool                              shutdown_{false};
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

    // ── submit() after shutdown throws ────────────────────────────────
    // pool destructor fires here, joining all workers
    // (submit after this would throw — pool is destroyed)

    std::cout << "\nDone — pool shut down cleanly.\n";
    return 0;
}
