#include <thread>
#include <mutex>
#include <chrono>
#include <shared_mutex>
#include <array>
#include <cstdio>

char ITEMS[5][15] = {"Laptops", "Monitors", "Keyboards", "Mice", "Printers"};
int item_stock[5] = {10, 15, 20, 25, 30};
std::shared_mutex inventory_mutex;

void inventory_reader(const int id) {
    for (int i = 0; i < 5; i++) {
        inventory_mutex.lock_shared();
        printf("Reader-%d sees %s stock is %d\n", id, ITEMS[i], item_stock[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        inventory_mutex.unlock_shared();
    }
}

void inventory_writer(const int id) {
    for (int i = 0; i < 5; i++) {
        inventory_mutex.lock();
        item_stock[i] = item_stock[i] + (id + 1); // Writers update stock
        printf("Writer-%d updated %s stock to %d\n", id, ITEMS[i], item_stock[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        inventory_mutex.unlock();
    }
}

int main() {
    // create ten reader threads but only two writer threads
    std::array<std::thread, 10> readers;
    for (unsigned int i = 0; i < readers.size(); i++) {
        readers[i] = std::thread(inventory_reader, i);
    }
    std::array<std::thread, 2> writers;
    for (unsigned int i = 0; i < writers.size(); i++) {
        writers[i] = std::thread(inventory_writer, i);
    }

    // wait for readers and writers to finish
    for (unsigned int i = 0; i < readers.size(); i++) {
        readers[i].join();
    }
    for (unsigned int i = 0; i < writers.size(); i++) {
        writers[i].join();
    }

    return 0;
}
