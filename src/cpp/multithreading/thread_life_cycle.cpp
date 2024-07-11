#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>

std::atomic<bool> working(true);

void task_worker(const char* name) {
    unsigned int task_count = 0;
    while (working) {
        task_count++;
    }
    std::cout << name << " completed " << task_count << " tasks.\n";
}

int main() {
    std::thread worker1(task_worker, "Alice");
    std::thread worker2(task_worker, "Bob");
    
    std::cout << "Alice and Bob are performing tasks...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    working = false;
    worker1.join();
    worker2.join();
    
    return 0;
}
