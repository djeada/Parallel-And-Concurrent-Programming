/**
 * Thread Starvation
 *
 * This example demonstrates starvation - when some threads get significantly
 * less CPU time or resources than others, even though they're not blocked.
 *
 * Key concepts:
 * - Starvation occurs when resource allocation is unfair
 * - Fast threads may dominate shared resources
 * - Slower threads may complete very few tasks
 * - Unlike deadlock, threads ARE making progress, just unevenly
 *
 * With many threads competing for a lock, some may be "starved" while
 * others repeatedly acquire it.
 */

#include <array>
#include <iostream>
#include <mutex>
#include <thread>

int task_count = 5000;
std::mutex task_lock;

void worker() {
    int tasks_completed = 0;

    while (true) {
        std::scoped_lock lock(task_lock);

        if (task_count > 0) {
            --task_count;
            ++tasks_completed;
        } else {
            break;
        }
    }

    std::cout << "Thread " << std::this_thread::get_id()
              << " completed " << tasks_completed << " tasks.\n";
}

int main() {
    // Many threads competing for the same resource
    std::array<std::thread, 200> workers;

    for (auto& w : workers) {
        w = std::thread(worker);
    }

    for (auto& w : workers) {
        w.join();
    }

    std::cout << "All workers done. Check task distribution above.\n";
    std::cout << "(Some threads may have done very few tasks - starvation!)\n";

    return 0;
}
