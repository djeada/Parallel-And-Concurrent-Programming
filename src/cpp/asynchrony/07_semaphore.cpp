#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <future>
#include <queue>
#include <condition_variable>
#include <shared_mutex>
#include <random>

class Semaphore {
public:
    explicit Semaphore(int count) : count(count) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return count > 0; });
        --count;
    }

    void release() {
        std::unique_lock<std::mutex> lock(mtx);
        ++count;
        cv.notify_one();
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

Semaphore resource_semaphore(3);

void limited_resource(int task_id) {
    resource_semaphore.acquire();
    std::cout << "Task " << task_id << " is using the limited resource.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 2000 + 500));
    std::cout << "Task " << task_id << " is done using the limited resource.\n";
    resource_semaphore.release();
}

int main() {
    std::vector<std::future<void>> tasks;

    for (int i = 0; i < 10; ++i) {
        tasks.push_back(std::async(std::launch::async, limited_resource, i));
    }

    for (auto &task : tasks) {
        task.wait();
    }

    return 0;
}
