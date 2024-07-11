#include <thread>
#include <mutex>
#include <cstdio>

unsigned int task_counter = 1; // start with one task completed
std::mutex counter_mutex;

void cpu_work(unsigned long workUnits) {
    unsigned long x = 0;
    for (unsigned long i = 0; i < workUnits * 1000000; i++) {
        x++;
    }
}

void worker_a() {
    cpu_work(1); // do a bit of work first
    std::scoped_lock<std::mutex> lock(counter_mutex);
    task_counter *= 2;
    printf("Worker A DOUBLED the task counter.\n");
}

void worker_b() {
    cpu_work(1); // do a bit of work first
    std::scoped_lock<std::mutex> lock(counter_mutex);
    task_counter += 3;
    printf("Worker B ADDED 3 to the task counter.\n");
}

int main() {
    std::thread workers[10];
    for (int i = 0; i < 10; i += 2) {
        workers[i] = std::thread(worker_a);
        workers[i + 1] = std::thread(worker_b);
    }
    for (auto& w : workers) {
        w.join();
    }
    printf("Total task counter is %u.\n", task_counter);
    return 0;
}
