/**
 * Spinlock with std::atomic_flag
 *
 * A spinlock achieves mutual exclusion through busy-waiting: instead of
 * blocking and yielding the CPU, a waiting thread loops continuously until
 * the lock is released. This avoids context-switch overhead but wastes
 * CPU cycles.
 *
 * std::atomic_flag is the only C++ atomic type guaranteed to be lock-free.
 * - test_and_set() returns the old value and atomically sets it to true
 * - clear() resets it to false
 *
 * When to use a spinlock:
 * - Critical section is extremely short (single cache-line write)
 * - Contention is very low
 * - Profiling shows mutex overhead is the bottleneck
 *
 * WARNING: Spinlocks waste CPU cycles while waiting. On single-core systems
 * or under high contention they can STARVE other threads.
 * Default to std::mutex and only switch to a spinlock with profiling evidence.
 *
 * Compile: g++ -std=c++11 -O2 -pthread spinlock.cpp
 */

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Spinlock {
public:
    void lock() noexcept {
        // Spin until we win the test-and-set race (old value was false)
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // Pause hint: on x86 emits the PAUSE instruction, reducing power
            // consumption and improving performance when spinning.
#if defined(__x86_64__) || defined(__i386__)
            __asm__ volatile("pause" ::: "memory");
#endif
        }
    }

    void unlock() noexcept {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

// ─── Example: protect a shared counter ───────────────────────────────────────

Spinlock spinlock;
long counter = 0;

void spinlock_worker() {
    for (int i = 0; i < 1000000; ++i) {
        spinlock.lock();
        ++counter;
        spinlock.unlock();
    }
}

// ─── Benchmark: spinlock vs std::mutex ───────────────────────────────────────

std::mutex std_mutex;
long mutex_counter = 0;

void mutex_worker() {
    for (int i = 0; i < 1000000; ++i) {
        std::lock_guard<std::mutex> lk(std_mutex);
        ++mutex_counter;
    }
}

template <typename F>
long benchmark_ms(F&& func, int num_threads) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_threads; ++i) threads.emplace_back(func);
    for (auto& t : threads) t.join();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start).count();
}

int main() {
    constexpr int kThreads = 4;

    long spin_ms  = benchmark_ms(spinlock_worker, kThreads);
    long mutex_ms = benchmark_ms(mutex_worker,    kThreads);

    std::cout << "Spinlock counter: " << counter       << "\n";
    std::cout << "Mutex counter:    " << mutex_counter << "\n";
    std::cout << "\nSpinlock time: " << spin_ms  << " ms\n";
    std::cout << "Mutex time:    " << mutex_ms << " ms\n";
    std::cout << "\nNote: for this workload (tiny critical section, moderate\n"
              << "contention) results vary. Spinlock wins only when contention\n"
              << "is near zero and the critical section is a handful of ns.\n";
    return 0;
}
