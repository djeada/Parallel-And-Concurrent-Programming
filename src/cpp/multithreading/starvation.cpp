#include <thread>
#include <mutex>
#include <array>
#include <iostream>

int task_count = 5000;
std::mutex task_lock;

void worker() {
    int tasks_completed = 0;
    while (true) {
        std::scoped_lock lock(task_lock);
        if (task_count > 0) {
            task_count--;
            tasks_completed++;
        } else {
            break;
        }
    }
    std::cout << "Worker " << std::this_thread::get_id() << " completed " << tasks_completed << " tasks.\n";
}

int main() {
    std::array<std::thread, 200> workers;
    for (auto& worker_thread : workers) {
        worker_thread = std::thread(worker);
    }
    for (auto& worker_thread : workers) {
        worker_thread.join();
    }
    std::cout << "The workers are done with their tasks.\n";
    return 0;
}
