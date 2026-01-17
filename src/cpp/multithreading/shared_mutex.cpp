/**
 * Shared Mutex (Reader-Writer Lock)
 *
 * This example demonstrates std::shared_mutex, which allows multiple
 * readers OR a single writer at any time.
 *
 * Key concepts:
 * - Multiple threads can hold a shared (read) lock simultaneously
 * - Only one thread can hold an exclusive (write) lock
 * - Writers wait for all readers to release before acquiring
 * - lock_shared() / unlock_shared() for readers
 * - lock() / unlock() for writers
 *
 * Use when reads are much more frequent than writes.
 */

#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

struct Inventory {
    std::array<std::string, 5> items = {
        "Laptops", "Monitors", "Keyboards", "Mice", "Printers"
    };
    std::array<int, 5> stock = {10, 15, 20, 25, 30};
    std::shared_mutex mutex;
};

Inventory inventory;

void reader(int id) {
    for (size_t i = 0; i < inventory.items.size(); ++i) {
        // Shared lock - multiple readers allowed
        std::shared_lock lock(inventory.mutex);

        std::cout << "Reader-" << id << " sees " << inventory.items[i]
                  << " stock: " << inventory.stock[i] << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void writer(int id) {
    for (size_t i = 0; i < inventory.items.size(); ++i) {
        // Exclusive lock - only one writer, no readers
        std::unique_lock lock(inventory.mutex);

        inventory.stock[i] += (id + 1);
        std::cout << "Writer-" << id << " updated " << inventory.items[i]
                  << " to: " << inventory.stock[i] << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::vector<std::thread> readers;
    std::vector<std::thread> writers;

    // Many readers, few writers (typical use case)
    for (int i = 0; i < 5; ++i) {
        readers.emplace_back(reader, i);
    }
    for (int i = 0; i < 2; ++i) {
        writers.emplace_back(writer, i);
    }

    for (auto& r : readers) {
        r.join();
    }
    for (auto& w : writers) {
        w.join();
    }

    std::cout << "All operations complete.\n";
    return 0;
}
