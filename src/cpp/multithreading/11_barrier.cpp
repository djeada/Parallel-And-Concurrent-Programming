#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Barrier {
public:
    explicit Barrier(size_t num_threads)
        : num_threads_(num_threads), count_(0), generation_(0) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        size_t gen = generation_;

        if (++count_ == num_threads_) {
            generation_++;
            count_ = 0;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t num_threads_;
    size_t count_;
    size_t generation_;
};

void worker(Barrier& barrier, int thread_id) {
    std::cout << "Thread " << thread_id << " is starting..." << std::endl;

    // Simulate some work
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(1, 3);
    std::this_thread::sleep_for(std::chrono::seconds(dist(rng)));

    std::cout << "Thread " << thread_id << " is waiting at the barrier..." << std::endl;

    barrier.wait(); // Wait for all threads to reach the barrier

    std::cout << "Thread " << thread_id << " is resuming after the barrier..." << std::endl;
}

int main() {
    size_t num_threads = 5;
    Barrier barrier(num_threads);

    std::vector<std::thread> threads;

    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, std::ref(barrier), i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "All threads have passed the barrier." << std::endl;

    return 0;
}
