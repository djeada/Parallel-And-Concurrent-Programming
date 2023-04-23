#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <random>
#include <deque>
#include <mutex>

int worker(int task_id) {
    std::cout << "Task " << task_id << " is starting..." << std::endl;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, 3);
    std::this_thread::sleep_for(std::chrono::seconds(dist(mt))); // Simulate some work
    int result = task_id * 2;
    std::cout << "Task " << task_id << " is finished. Result: " << result << std::endl;
    return result;
}

int main() {
    const int num_tasks = 10;
    const int num_workers = 3;
    std::vector<std::future<int>> futures;

    // Create a thread pool
    {
        std::vector<std::thread> threads;
        std::deque<int> task_ids;
        std::mutex task_ids_mutex;
        std::vector<std::packaged_task<int(int)>> tasks(num_tasks);

        // Initialize tasks and task_ids
        for (int i = 0; i < num_tasks; ++i) {
            tasks[i] = std::packaged_task<int(int)>(worker);
            task_ids.push_back(i);
        }

        // Worker function
        auto worker_fn = [&]() {
            while (true) {
                int task_id;
                {
                    std::lock_guard<std::mutex> lock(task_ids_mutex);
                    if (task_ids.empty()) {
                        break;
                    }
                    task_id = task_ids.front();
                    task_ids.pop_front();
                }
                tasks[task_id].operator()(task_id);
            }
        };

        // Launch worker threads
        for (int i = 0; i < num_workers; ++i) {
            threads.emplace_back(worker_fn);
        }

        // Join worker threads
        for (auto& thread : threads) {
            thread.join();
        }

        // Retrieve futures
        for (auto& task : tasks) {
            futures.push_back(task.get_future());
        }
    }

    // Collect the results as the tasks complete
    for (int i = 0; i < num_tasks; ++i) {
        int result = futures[i].get();
        std::cout << "Task " << i << " result collected: " << result << std::endl;
    }

    return 0;
}
