#include <thread>
#include <mutex>
#include <cstdio>

int task_count = 5000;

void worker(std::mutex &task_lock) {
    while (task_count > 0) {
        std::scoped_lock lock(task_lock);
        if (task_count > 0) {
            task_count--;
        }
        if (task_count == 10) {
            printf("This worker has had enough tasks!\n");
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
    printf("The workers are done with their tasks.\n");
    return 0;
}
