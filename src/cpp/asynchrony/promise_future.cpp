/**
 * std::promise and std::future Communication
 *
 * This example demonstrates using std::promise and std::future
 * to pass data between async tasks.
 *
 * Key concepts:
 * - std::promise is the "sender" side - sets the value
 * - std::future is the "receiver" side - gets the value
 * - promise.set_value() makes the value available
 * - future.get() blocks until value is set, then returns it
 * - One-shot communication: set and get can only be called once
 *
 * This is the fundamental building block for async data transfer.
 */

#include <chrono>
#include <future>
#include <iostream>
#include <random>
#include <thread>

int produce_data() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    int data = dist(gen);
    std::cout << "Producer: created data = " << data << "\n";
    return data;
}

void consume_data(int data) {
    std::cout << "Consumer: received data = " << data << "\n";
}

int main() {
    // Create promise/future pair for communication
    std::promise<int> data_promise;
    std::future<int> data_future = data_promise.get_future();

    // Producer task: generates data and sends via promise
    auto producer = std::async(std::launch::async, [&data_promise]() {
        int data = produce_data();
        data_promise.set_value(data);
    });

    // Consumer task: receives data via future
    auto consumer = std::async(std::launch::async, [&data_future]() {
        int data = data_future.get();  // Blocks until producer sets value
        consume_data(data);
    });

    producer.wait();
    consumer.wait();

    std::cout << "Communication complete\n";
    return 0;
}
