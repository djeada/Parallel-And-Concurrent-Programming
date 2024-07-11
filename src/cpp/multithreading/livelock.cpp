#include <thread>
#include <mutex>
#include <iostream>
#include <vector>

// Global task counter
int task_count = 5000;

// Worker function
void worker(std::mutex &first_tool, std::mutex &second_tool, const std::string &worker_name) {
    while (task_count > 0) {
        first_tool.lock();
        if (!second_tool.try_lock()) {
            first_tool.unlock();
            std::cout << worker_name << " is yielding to avoid livelock." << std::endl;
            std::this_thread::yield();
        } else {
            if (task_count > 0) {
                task_count--;
                std::cout << worker_name << " completed a task. Remaining: " << task_count << std::endl;
            }
            second_tool.unlock();
            first_tool.unlock();
        }
    }
}

int main() {
    // Mutexes representing tools
    std::mutex tool_a, tool_b;

    // Worker threads
    std::vector<std::thread> workers;
    workers.emplace_back(worker, std::ref(tool_a), std::ref(tool_b), "Alice");
    workers.emplace_back(worker, std::ref(tool_b), std::ref(tool_a), "Bob");
    workers.emplace_back(worker, std::ref(tool_a), std::ref(tool_b), "Charlie");
    workers.emplace_back(worker, std::ref(tool_b), std::ref(tool_a), "Diana");

    // Join threads
    for (auto &worker : workers) {
        worker.join();
    }

    std::cout << "The workers have completed their tasks." << std::endl;
    return 0;
}
