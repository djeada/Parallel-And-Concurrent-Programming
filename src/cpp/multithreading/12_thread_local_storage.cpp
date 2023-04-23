#include <iostream>
#include <thread>
#include <vector>

// Declare a thread_local variable
thread_local int thread_local_data;

void some_work() {
    thread_local_data *= 2;
}

void worker(int thread_id) {
    // Store thread-local data
    thread_local_data = thread_id;

    // Simulate some work
    some_work();

    // Read thread-local data
    std::cout << "Thread " << thread_id << " has value: " << thread_local_data << std::endl;
}

int main() {
    const int num_threads = 5;
    std::vector<std::thread> threads;

    // Create and start worker threads
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(worker, i));
    }

    // Join worker threads
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
