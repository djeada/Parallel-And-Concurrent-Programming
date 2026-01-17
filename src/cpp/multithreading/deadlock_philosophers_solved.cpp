/**
 * Deadlock Solution - Consistent Lock Ordering
 *
 * This example shows how to avoid deadlock by having threads release
 * resources when they can't acquire all needed resources.
 *
 * Key concepts:
 * - If a thread can't acquire a lock, it should release held locks
 *   and retry (prevents hold-and-wait condition)
 * - Using scoped_lock (shown here) atomically acquires multiple locks
 *
 * Note: The pattern shown here (early exit when condition is met)
 * prevents a single thread from monopolizing resources.
 */

#include <iostream>
#include <mutex>
#include <thread>

int task_count = 5000;

void worker(std::mutex& task_lock) {
    while (task_count > 0) {
        std::scoped_lock lock(task_lock);

        if (task_count > 0) {
            --task_count;
        }

        // Yield early to give other threads a chance
        if (task_count == 10) {
            std::cout << "Worker reached threshold, yielding.\n";
            break;
        }
    }
}

int main() {
    std::mutex task_lock;

    std::thread alice(worker, std::ref(task_lock));
    std::thread bob(worker, std::ref(task_lock));

    alice.join();
    bob.join();

    std::cout << "Workers completed their tasks.\n";
    return 0;
}
