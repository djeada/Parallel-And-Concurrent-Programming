#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <condition_variable>

// Global task counter
unsigned int task_counter = 1; // start with one task completed
std::mutex counter_mutex;

// Synchronization variables
std::mutex latch_mutex;
std::condition_variable latch_cv;
int latch_count = 10;

void cpu_work(unsigned long workUnits) {
    unsigned long x = 0;
    for (unsigned long i = 0; i < workUnits * 1000000; i++) {
        x++;
    }
}

// Worker A function
void worker_a() {
    cpu_work(1); // Perform some initial work
    
    {
        std::unique_lock<std::mutex> latch_lock(latch_mutex);
        latch_cv.wait(latch_lock, []{ return latch_count == 0; });
    }

    {
        std::scoped_lock lock(counter_mutex);
        task_counter *= 2; // Double the task counter
        std::cout << "Worker A DOUBLED the task counter.\n";
    }
}

// Worker B function
void worker_b() {
    cpu_work(1); // Perform some initial work
    
    {
        std::scoped_lock lock(counter_mutex);
        task_counter += 3; // Add 3 to the task counter
        std::cout << "Worker B ADDED 3 to the task counter.\n";
    }

    {
        std::scoped_lock latch_lock(latch_mutex);
        if (--latch_count == 0) {
            latch_cv.notify_all();
        }
    }
}

int main() {
    // Create a vector to hold worker threads
    std::vector<std::thread> workers;

    // Launch worker threads
    for (int i = 0; i < 10; i += 2) {
        workers.emplace_back(worker_a);
        workers.emplace_back(worker_b);
    }

    // Join all threads
    for (auto& worker : workers) {
        worker.join();
    }

    // Output the final task counter value
    std::cout << "Total task counter is " << task_counter << ".\n";

    return 0;
}
