/**
 * Mutex Contention with Multiple Threads
 *
 * This example shows how multiple threads contend for a single mutex.
 * All shared state is only accessed inside the lock — no data races.
 *
 * Key concepts:
 * - Use while(true) + check-and-break inside the lock to avoid reading
 *   shared state outside the critical section (prevents data races)
 * - When many threads compete for one mutex, some will starve while others
 *   repeatedly win (see starvation.cpp for a dedicated example)
 * - A thread may exit early without processing all remaining tasks
 *
 * Compile: g++ -std=c++11 -pthread mutex_contention.cpp
 */

#include <iostream>
#include <mutex>
#include <thread>

int task_count = 5000;

void worker(int id, std::mutex& task_lock) {
    int processed = 0;
    while (true) {
        std::scoped_lock lock(task_lock);

        if (task_count <= 0) break;
        --task_count;
        ++processed;

        // Early exit: worker 0 stops at a threshold to illustrate
        // that threads can leave without draining the whole queue.
        if (id == 0 && task_count == 100) {
            std::cout << "Worker 0 reached threshold and exited early.\n";
            break;
        }
    }
    std::cout << "Worker " << id << " processed " << processed << " tasks.\n";
}

int main() {
    std::mutex task_lock;

    std::thread alice(worker, 0, std::ref(task_lock));
    std::thread bob(worker,   1, std::ref(task_lock));

    alice.join();
    bob.join();

    std::cout << "Tasks remaining: " << task_count << "\n";
    return 0;
}
