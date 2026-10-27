/**
 * C++20 + POSIX: a process-shared mutex with scoped locking.
 * pthread_mutex_t lives in a shared mapping and uses PTHREAD_PROCESS_SHARED.
 * std::lock_guard owns each critical section; children finish before cleanup.
 * An ordinary std::mutex is not a process-shared substitute.
 * Compile: g++ -std=c++20 -pthread process_mutex.cpp
 */
#include "../common/posix.hpp"

#include <iostream>
#include <mutex>
#include <pthread.h>

struct SharedData {
    pthread_mutex_t mutex;
    int counter = 0;
};

class ProcessMutex {
public:
    explicit ProcessMutex(pthread_mutex_t& mutex) : mutex_(mutex) {
        pthread_mutexattr_t attributes;
        check(pthread_mutexattr_init(&attributes));
        const int shared = pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);
        const int initialized = shared == 0 ? pthread_mutex_init(&mutex_, &attributes) : shared;
        pthread_mutexattr_destroy(&attributes);
        check(initialized);
    }
    ~ProcessMutex() { pthread_mutex_destroy(&mutex_); }
    ProcessMutex(const ProcessMutex&) = delete;
    ProcessMutex& operator=(const ProcessMutex&) = delete;
    void lock() { check(pthread_mutex_lock(&mutex_)); }
    void unlock() noexcept { pthread_mutex_unlock(&mutex_); }

private:
    static void check(int code) {
        if (code != 0) throw std::system_error(code, std::generic_category(), "pthread mutex");
    }
    pthread_mutex_t& mutex_;
};

int main() try {
    constexpr int num_workers = 4;
    posix_example::SharedObject<SharedData> data;
    ProcessMutex mutex(data->mutex);
    posix_example::ChildGroup children(num_workers);
    for (int id = 0; id < num_workers; ++id) {
        children.spawn([&, id] {
            for (int i = 0; i < 1000; ++i) {
                std::lock_guard lock(mutex);
                ++data->counter;
            }
            std::cout << "Worker " << id << " finished\n";
        });
    }
    if (!children.wait_all()) return 1;
    std::cout << "Final counter: " << data->counter << "\nExpected: 4000\n";
    return data->counter == 4000 ? 0 : 1;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
