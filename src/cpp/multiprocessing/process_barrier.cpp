/**
 * C++20 + POSIX: use a process-shared platform barrier.
 * pthread_barrier_t is stored in shared memory with PTHREAD_PROCESS_SHARED.
 * Unlike std::barrier, this API supports synchronization across processes.
 * All participants must arrive; a failed startup terminates/reaps started peers.
 * Compile: g++ -std=c++20 -pthread process_barrier.cpp
 */
#include "../common/posix.hpp"

#include <chrono>
#include <iostream>
#include <pthread.h>
#include <thread>

class ProcessBarrier {
public:
    ProcessBarrier(pthread_barrier_t& barrier, unsigned int count) : barrier_(barrier) {
        pthread_barrierattr_t attributes;
        check(pthread_barrierattr_init(&attributes));
        const int shared = pthread_barrierattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);
        const int initialized = shared == 0 ? pthread_barrier_init(&barrier_, &attributes, count) : shared;
        pthread_barrierattr_destroy(&attributes);
        check(initialized);
    }
    ~ProcessBarrier() { pthread_barrier_destroy(&barrier_); }
    ProcessBarrier(const ProcessBarrier&) = delete;
    ProcessBarrier& operator=(const ProcessBarrier&) = delete;
    void wait() {
        const int code = pthread_barrier_wait(&barrier_);
        if (code != PTHREAD_BARRIER_SERIAL_THREAD) check(code);
    }

private:
    static void check(int code) {
        if (code != 0) throw std::system_error(code, std::generic_category(), "pthread barrier");
    }
    pthread_barrier_t& barrier_;
};

int main() try {
    constexpr int num_workers = 4;
    posix_example::SharedObject<pthread_barrier_t> state;
    ProcessBarrier barrier(*state.get(), num_workers);
    posix_example::ChildGroup children(num_workers);
    for (int id = 0; id < num_workers; ++id) {
        children.spawn([&, id] {
            std::cout << "Worker " << id << " doing phase 1 work...\n" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(20 * id));
            barrier.wait();
            std::cout << "Worker " << id << " passed barrier, doing phase 2\n";
        });
    }
    if (!children.wait_all()) return 1;
    std::cout << "All workers finished\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
