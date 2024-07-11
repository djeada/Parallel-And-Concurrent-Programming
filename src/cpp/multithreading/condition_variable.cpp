#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdio>

int task_count = 10;
std::mutex task_pool_lock;
std::condition_variable task_available;

void worker(int id) {
    int yielded_turns = 0;
    while (task_count > 0) {
        std::unique_lock<std::mutex> lock(task_pool_lock); // lock the task pool
        while ((id != task_count % 5) && (task_count > 0)) { // is it your turn to process a task?
            yielded_turns++; // it's not your turn; yield...
            task_available.wait(lock); // ...and wait...
        }
        if (task_count > 0) {
            task_count--; // it's your turn; process a task!
            lock.unlock(); // release the lock
            task_available.notify_all(); // notify other threads that a task has been taken
        }
    }
    printf("Worker %d yielded %u times.\n", id, yielded_turns);
}

int main() {
    std::thread worker_threads[5];
    for (int i = 0; i < 5; i++) {
        worker_threads[i] = std::thread(worker, i);
    }
    for (auto& wt : worker_threads) {
        wt.join();
    }
    return 0;
}
