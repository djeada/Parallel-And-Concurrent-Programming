/*
This C++ program demonstrates data sharing and communication between asynchronous tasks
using the C++ standard library. In this example, we simulate a producer and consumer
scenario where one task produces data and the other consumes it using std::promise and
std::future for communication and synchronization.
*/

#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <random>

int produce_data() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 100);
    int data = distr(gen);
    std::cout << "Produced data: " << data << std::endl;
    return data;
}

void consume_data(int data) {
    std::cout << "Consumed data: " << data << std::endl;
}

int main() {
    std::promise<int> data_promise;
    std::future<int> data_future = data_promise.get_future();

    // Producer task
    auto producer_task = std::async(std::launch::async, [&data_promise]() {
        int data = produce_data();
        data_promise.set_value(data);
    });

    // Consumer task
    auto consumer_task = std::async(std::launch::async, [&data_future]() {
        int data = data_future.get();
        consume_data(data);
    });

    producer_task.wait();
    consumer_task.wait();

    return 0;
}
