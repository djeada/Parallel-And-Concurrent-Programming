#include <thread>
#include <mutex>

unsigned int document_count = 0;
unsigned int report_count = 0;
std::recursive_mutex counter_mutex;

void add_document() {
    counter_mutex.lock();
    document_count++;
    counter_mutex.unlock();
}

void add_report() {
    counter_mutex.lock();
    report_count++;
    add_document();
    counter_mutex.unlock();
}

void worker() {
    for (int i = 0; i < 10000; i++) {
        add_document();
        add_report();
    }
}

int main() {
    std::thread alice(worker);
    std::thread bob(worker);
    alice.join();
    bob.join();
    printf("We processed %u documents.\n", document_count);
    printf("We generated %u reports.\n", report_count);

    return 0;
}
