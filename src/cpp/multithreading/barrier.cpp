#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <condition_variable>
#include <atomic>

// Global task counter
unsigned int task_counter = 1; // start with one task completed
std::mutex counter_mutex;

// Barrier implementation using condition variable
class Barrier {
public:
    explicit Barrier(std::size_t count) : threshold(count), count(count), generation(0) {}
    void wait();

private:
    std::mutex mutex;
    std::condition_variable cv;
    const std::size_t threshold;
    std::size_t count;
    std::size_t generation;
};

void Barrier::wait() {
    std::unique_lock<std::mutex> lock(mutex);
    auto gen = generation;
    if (--count == 0) {
        generation++;
        count = threshold;
        cv.notify_all();
    } else {
        cv.wait(lock, [this, gen] { return gen != generation; });
    }
}

// Simulate CPU work
void cpu_work(unsigned long workUnits) {
    unsigned long x = 0;
    for (unsigned long i = 0; i < workUnits * 1000000; i++) {
        x++;
    }
}

// Worker A function
void worker_a(Barrier &barrier) {
    cpu_work(1); // Perform some initial work
    barrier.wait(); // Wait for other threads to reach this point
    {
        std::scoped_lock lock(counter_mutex);
        task_counter *= 2; // Double the task counter
        std::cout << "Worker A DOUBLED the task counter.\n";
    }
}

// Worker B function
void worker_b(Barrier &barrier) {
    cpu_work(1); // Perform some initial work
    {
        std::scoped_lock lock(counter_mutex);
        task_counter += 3; // Add 3 to the task counter
        std::cout << "Worker B ADDED 3 to the task counter.\n";
    }
    barrier.wait(); // Wait for other threads to reach this point
}

int main() {
    Barrier sync_point(10); // Barrier for 10 threads

    // Create a vector to hold worker threads
    std::vector<std::thread> workers;

    // Launch worker threads
    for (int i = 0; i < 10; i += 2) {
        workers.emplace_back(worker_a, std::ref(sync_point));
        workers.emplace_back(worker_b, std::ref(sync_point));
    }

    // Join all threads
    for (auto& worker : workers) {
        worker.join();
    }

    // Output the final task counter value
    std::cout << "Total task counter is " << task_counter << ".\n";

    return 0;
}
