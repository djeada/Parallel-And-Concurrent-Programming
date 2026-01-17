/**
 * Async Producer-Consumer Pattern
 *
 * This example demonstrates the producer-consumer pattern using
 * async tasks and a thread-safe queue.
 *
 * Key concepts:
 * - Thread-safe queue with mutex and condition_variable
 * - Multiple producers and consumers can run concurrently
 * - put() notifies waiting consumers
 * - get() blocks until an item is available
 *
 * This pattern decouples data production from consumption,
 * enabling pipeline-style processing.
 */

#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <vector>

class AsyncQueue {
public:
    void put(const std::string& item) {
        {
            std::scoped_lock lock(mutex_);
            queue_.push(item);
        }
        cv_.notify_one();
    }

    std::string get() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        std::string item = queue_.front();
        queue_.pop();
        return item;
    }

private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

void producer(std::shared_ptr<AsyncQueue> queue, int id, int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(200, 800);

    for (int i = 0; i < count; ++i) {
        std::string item = "P" + std::to_string(id) + "-Item" + std::to_string(i);
        queue->put(item);
        std::cout << "Producer " << id << ": " << item << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    }
}

void consumer(std::shared_ptr<AsyncQueue> queue, int id, int count) {
    for (int i = 0; i < count; ++i) {
        std::string item = queue->get();
        std::cout << "Consumer " << id << ": " << item << "\n";
    }
}

int main() {
    auto queue = std::make_shared<AsyncQueue>();

    constexpr int num_producers = 3;
    constexpr int items_per_producer = 4;
    constexpr int num_consumers = 2;
    constexpr int items_per_consumer = 6;  // Total: 12 items produced, 12 consumed

    std::vector<std::future<void>> producers;
    for (int i = 0; i < num_producers; ++i) {
        producers.push_back(
            std::async(std::launch::async, producer, queue, i, items_per_producer));
    }

    std::vector<std::future<void>> consumers;
    for (int i = 0; i < num_consumers; ++i) {
        consumers.push_back(
            std::async(std::launch::async, consumer, queue, i, items_per_consumer));
    }

    for (auto& p : producers) p.get();
    for (auto& c : consumers) c.get();

    std::cout << "All done\n";
    return 0;
}
