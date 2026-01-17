/**
 * Recursive Mutex
 *
 * This example demonstrates std::recursive_mutex, which allows the same
 * thread to acquire the mutex multiple times without deadlocking.
 *
 * Key concepts:
 * - std::recursive_mutex can be locked multiple times by the SAME thread
 * - Each lock() must be matched with an unlock()
 * - Useful when a function calls another function that also needs the lock
 * - Regular std::mutex would deadlock in this scenario
 *
 * Use case: Nested function calls that both need to protect shared data.
 */

#include <iostream>
#include <mutex>
#include <thread>

unsigned int document_count = 0;
unsigned int report_count = 0;
std::recursive_mutex counter_mutex;

void add_document() {
    std::scoped_lock lock(counter_mutex);  // Lock (may already be held)
    ++document_count;
}

void add_report() {
    std::scoped_lock lock(counter_mutex);  // Lock
    ++report_count;
    add_document();  // This would deadlock with regular mutex!
}

void worker() {
    for (int i = 0; i < 10000; ++i) {
        add_document();  // Acquires lock once
        add_report();     // Acquires lock, then add_document acquires again
    }
}

int main() {
    std::thread alice(worker);
    std::thread bob(worker);

    alice.join();
    bob.join();

    // Each iteration: 1 document + 1 report + 1 document = 3 documents, 1 report
    std::cout << "Documents processed: " << document_count << "\n";  // 40000
    std::cout << "Reports generated:   " << report_count << "\n";    // 20000

    return 0;
}
