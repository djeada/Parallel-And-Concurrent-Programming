#include <future>
#include <cstdio>
#include <thread>
#include <chrono>

int process_task() {
    printf("Worker is processing tasks...\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 42;
}

int main() {
    printf("Manager asks Worker how many tasks have been processed.\n");
    std::future<int> result = std::async(std::launch::async, process_task);
    printf("Manager can do other things while waiting for the result...\n");
    printf("Worker responded with %d tasks completed.\n", result.get());
    return 0;
}
