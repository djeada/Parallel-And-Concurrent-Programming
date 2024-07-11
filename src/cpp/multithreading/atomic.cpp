#include <thread>
#include <atomic>

std::atomic<unsigned int> task_count(0);

void worker() {
    for (int i = 0; i < 10000000; i++) {
        task_count++;
    }
}

int main() {
    std::thread alice(worker);
    std::thread bob(worker);
    alice.join();
    bob.join();
    printf("We completed %u tasks.\n", task_count.load());

    return 0;
}
