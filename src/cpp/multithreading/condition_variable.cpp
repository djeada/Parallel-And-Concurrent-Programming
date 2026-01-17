/**
 * Condition Variables
 *
 * This example demonstrates std::condition_variable, which allows threads
 * to wait efficiently until a specific condition becomes true.
 *
 * Key concepts:
 * - condition_variable.wait() releases the mutex and blocks the thread
 * - When notified, it reacquires the mutex and checks the condition
 * - notify_one() wakes one waiting thread; notify_all() wakes all
 * - Always use a predicate with wait() to handle spurious wakeups
 *
 * This example shows round-robin task distribution where each worker
 * only processes tasks assigned to their ID.
 */

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int task_count = 10;
std::mutex task_mutex;
std::condition_variable task_available;

void worker(int id) {
    int tasks_completed = 0;
    int yielded_turns = 0;

    while (true) {
        std::unique_lock<std::mutex> lock(task_mutex);

        // Wait until it's this worker's turn OR no tasks remain
        // The predicate handles spurious wakeups automatically
        task_available.wait(lock, [id] {
            return (task_count % 5 == id) || (task_count <= 0);
        });

        if (task_count <= 0) {
            break;  // No more tasks
        }

        // Process the task
        --task_count;
        ++tasks_completed;

        lock.unlock();
        task_available.notify_all();  // Wake other workers to check their turn
    }

    std::cout << "Worker " << id << " completed " << tasks_completed
              << " tasks.\n";
}

int main() {
    constexpr int num_workers = 5;
    std::vector<std::thread> workers;
    workers.reserve(num_workers);

    for (int i = 0; i < num_workers; ++i) {
        workers.emplace_back(worker, i);
    }

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "All tasks completed.\n";
    return 0;
}
