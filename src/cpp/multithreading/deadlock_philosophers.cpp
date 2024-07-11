#include <thread>
#include <mutex>
#include <cstdio>

int item_count = 5000;

void worker(std::mutex &first_bin, std::mutex &second_bin) {
    while (item_count > 0) {
        std::scoped_lock lock(first_bin, second_bin);
        if (item_count > 0) {
            item_count--;
        }
    }
}

int main() {
    std::mutex bin_a, bin_b;
    std::thread alice(worker, std::ref(bin_a), std::ref(bin_b));
    std::thread bob(worker, std::ref(bin_b), std::ref(bin_a));
    alice.join();
    bob.join();
    printf("The workers are done picking items.\n");
    return 0;
}
