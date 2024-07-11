#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>

class Semaphore {
public:
    Semaphore(unsigned long init_count) {
        count_ = init_count;
    }

    void acquire() { // decrement the internal counter
        std::unique_lock<std::mutex> lck(m_);
        while (!count_) {
            cv_.wait(lck);
        }
        count_--;
    }

    void release() { // increment the internal counter
        std::unique_lock<std::mutex> lck(m_);
        count_++;
        lck.unlock();
        cv_.notify_one();
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    unsigned long count_;
};

Semaphore printer(1);

void office_worker(int id) {
    printer.acquire();
    printf("Worker %d is using the printer...\n", id);
    srand(id); // use the printer for a "random" amount between 1-3 seconds
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 2000 + 1000));
    printf("Worker %d is DONE using the printer!\n", id);
    printer.release();
}

int main() {
    std::thread workers[10];
    for (int i = 0; i < 10; i++) {
        workers[i] = std::thread(office_worker, i);
    }
    for (auto& w : workers) {
        w.join();
    }
    return 0;
}
