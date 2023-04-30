/*
This script demonstrates the producer-consumer pattern using C++'s standard library with std::async.
The producer creates items and puts them into a queue, while the consumer takes items from the queue and processes them.
This pattern is useful when you want to separate the creation and processing of items, allowing them to run concurrently.
*/

#include <iostream>
#include <future>
#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <random>
#include <thread>

class AsyncQueue {
public:
    void put(const std::string& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_condition.notify_one();
    }

    std::string get() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [&] { return !m_queue.empty(); });
        std::string item = m_queue.front();
        m_queue.pop();
        return item;
    }

private:
    std::queue<std::string> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
};

void producer(std::shared_ptr<AsyncQueue> queue, int producer_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(500, 2000);

    for (int i = 0; i < 5; ++i) {
        std::string item = "Item " + std::to_string(i) + " from producer " + std::to_string(producer_id);
        queue->put(item);
        std::cout << "Producer " << producer_id << " produced " << item << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dis(gen))));
    }
}

void consumer(std::shared_ptr<AsyncQueue> queue, int consumer_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(500, 2000);

    for (int i = 0; i < 5; ++i) {
        std::string item = queue->get();
        std::cout << "Consumer " << consumer_id << " consumed " << item << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(dis(gen))));
    }
}

int main() {
    auto queue = std::make_shared<AsyncQueue>();

    std::vector<std::future<void>> producers;
    for (int i = 0; i < 3; ++i) {
        producers.push_back(std::async(std::launch::async, producer, queue, i));
    }

    std::vector<std::future<void>> consumers;
    for (int i = 0; i < 2; ++i) {
        consumers.push_back(std::async(std::launch::async, consumer, queue, i));
    }

    for (auto& producer_future : producers) {
        producer_future.wait();
    }

    for (auto& consumer_future : consumers) {
        consumer_future.wait();
    }

    return 0;
}
