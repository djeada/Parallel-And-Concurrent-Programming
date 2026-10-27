/**
 * C++20: future-based producers and consumers sharing a bounded queue.
 * Capacity applies backpressure; close() ends consumption after draining.
 * Consumers do not need to know how many items will arrive. Queue waits block
 * worker threads; use parallel_fetch.cpp for event-driven asynchronous I/O.
 * Compile: g++ -std=c++20 -pthread async_producer_consumer.cpp
 */
#include "../common/bounded_queue.hpp"

#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <syncstream>
#include <vector>

using AsyncQueue = cpp_example::BoundedQueue<std::string>;

void producer(AsyncQueue& queue, int id) {
    for (int item = 0; item < 4; ++item) {
        auto value = "P" + std::to_string(id) + "-Item" + std::to_string(item);
        if (!queue.add_task(std::move(value))) return;
    }
}

void consumer(AsyncQueue& queue, int id) {
    int count = 0;
    while (auto value = queue.take_task()) {
        ++count;
        std::osyncstream(std::cout) << "Consumer " << id << ": " << *value << '\n';
    }
    std::osyncstream(std::cout) << "Consumer " << id << " consumed " << count << " items\n";
}

int main() try {
    AsyncQueue queue(4); // Outlives every future and worker that references it.
    std::vector<std::future<void>> consumers;
    std::vector<std::future<void>> producers;
    try {
        for (int id = 0; id < 2; ++id) {
            consumers.push_back(std::async(std::launch::async, consumer, std::ref(queue), id));
        }
        for (int id = 0; id < 3; ++id) {
            producers.push_back(std::async(std::launch::async, producer, std::ref(queue), id));
        }
        for (auto& future : producers) future.get();
    } catch (...) {
        queue.close(); // Release waiters before futures wait during unwinding.
        throw;
    }
    queue.close();
    for (auto& future : consumers) future.get();
    std::cout << "All done\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
