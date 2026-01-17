/**
 * Producer-Consumer Pattern
 *
 * This example demonstrates the producer-consumer pattern using a
 * thread-safe queue. Producers add items, consumers take them.
 *
 * Key concepts:
 * - Thread-safe queue using mutex + condition_variable
 * - Producers signal consumers when items are added
 * - Consumers block waiting for items
 * - Poison pill pattern (-1) signals termination
 * - Multiple consumers can process the same queue
 */

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class TaskQueue {
public:
    void add_task(int task) {
        {
            std::scoped_lock lock(mutex_);
            queue_.push(task);
        }
        cv_.notify_one();
    }

    int take_task() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });

        int task = queue_.front();
        queue_.pop();
        return task;
    }

private:
    std::queue<int> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

TaskQueue task_queue;

void producer() {
    // Add tasks
    for (int i = 0; i < 10000; ++i) {
        task_queue.add_task(1);
    }

    // Signal end with poison pill
    task_queue.add_task(-1);
    std::cout << "Producer: finished adding tasks.\n";
}

void consumer(int id) {
    int tasks_completed = 0;

    while (true) {
        int task = task_queue.take_task();

        if (task == -1) {
            // Put poison pill back for other consumers
            task_queue.add_task(-1);
            break;
        }

        tasks_completed += task;
    }

    std::cout << "Consumer " << id << ": completed " << tasks_completed
              << " tasks.\n";
}

int main() {
    std::thread prod(producer);
    std::thread cons1(consumer, 1);
    std::thread cons2(consumer, 2);

    prod.join();
    cons1.join();
    cons2.join();

    return 0;
}
