#include <iostream>
#include <chrono>
#include <random>
#include <thread>
#include <vector>
#include <barrier>
#include <coroutine>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 5);

class task_worker {
public:
    task_worker(int task_id, std::barrier<>* barrier)
        : task_id_(task_id), barrier_(barrier) {}

    void operator()() {
        phase_1();
        barrier_->arrive_and_wait();
        phase_2();
    }

private:
    void phase_1() {
        std::cout << "Task " << task_id_ << " starting phase 1..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(dis(gen)));
        std::cout << "Task " << task_id_ << " finished phase 1" << std::endl;
    }

    void phase_2() {
        std::cout << "Task " << task_id_ << " starting phase 2..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(dis(gen)));
        std::cout << "Task " << task_id_ << " finished phase 2" << std::endl;
    }

    int task_id_;
    std::barrier<>* barrier_;
};

int main() {
    const int num_tasks = 5;
    std::barrier<> barrier(num_tasks);
    std::vector<std::thread> tasks;

    for (int i = 0; i < num_tasks; ++i) {
        tasks.emplace_back(task_worker(i, &barrier));
    }

    for (auto& task : tasks) {
        task.join();
    }

    return 0;
}
