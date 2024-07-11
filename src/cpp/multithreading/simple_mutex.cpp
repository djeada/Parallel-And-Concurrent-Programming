#include <thread>
#include <mutex>
#include <chrono>

unsigned int task_count = 0;
std::mutex counter_mutex;

void worker() {
    for (int i = 0; i < 5; i++) {
        printf("Worker %d is planning...\n", std::this_thread::get_id());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        counter_mutex.lock();
        task_count++;
        counter_mutex.unlock();
    }
}

int main() {
    std::thread alice(worker);
    std::thread bob(worker);
    alice.join();
    bob.join();
    printf("We completed %u tasks.\n", task_count);

    return 0;
}
