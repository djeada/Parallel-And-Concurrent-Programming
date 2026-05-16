/**
 * False Sharing - Cache Line Contention
 *
 * ANTIPATTERN: When two threads repeatedly write to variables that happen
 * to reside on the same CPU cache line, every write by one thread
 * invalidates the cache line in the other CPU — even though the threads
 * access different variables. This "false sharing" turns a parallel write
 * into essentially sequential work.
 *
 * CPU cache lines are typically 64 bytes wide. If counter_a and counter_b
 * sit adjacent in memory and fit in the same cache line, each increment
 * by thread A forces thread B's cache to reload the line, and vice versa.
 *
 * Fix: Pad each variable to its own cache line using alignas so that no
 * two threads' hot variables share a cache line.
 *
 * WARNING: This code intentionally demonstrates the false sharing antipattern
 * in the first benchmark. The second benchmark shows the corrected version.
 *
 * Compile: g++ -std=c++17 -O2 -pthread false_sharing.cpp
 */

#include <atomic>
#include <chrono>
#include <iostream>
#include <new>
#include <thread>

constexpr long kIterations = 200'000'000L;

// Use the compiler-provided cache line size when available (C++17),
// otherwise fall back to the common 64-byte value.
#ifdef __cpp_lib_hardware_interference_size
constexpr std::size_t kCacheLineSize = std::hardware_destructive_interference_size;
#else
constexpr std::size_t kCacheLineSize = 64;
#endif

// ─── ANTIPATTERN: two counters share one cache line ──────────────────────────
//
// sizeof(long) is typically 8 bytes; two longs fit easily in 64 bytes.
// Thread A and Thread B's writes thrash the same cache line.

struct SharedCacheLine {
    std::atomic<long> counter_a{0};  // Thread A writes this
    std::atomic<long> counter_b{0};  // Thread B writes this — same cache line!
};

// ─── FIX: each counter lives on its own dedicated cache line ─────────────────

struct alignas(kCacheLineSize) PaddedCounter {
    std::atomic<long> value{0};
    // Implicit padding fills the rest of the cache line.
    // The next PaddedCounter starts on a fresh cache line.
};

struct PaddedCacheLines {
    PaddedCounter counter_a;  // Thread A — exclusive cache line
    PaddedCounter counter_b;  // Thread B — exclusive cache line
};

// Benchmark helper
template <typename F>
long benchmark_ms(F&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    std::cout << "Cache line size: " << kCacheLineSize << " bytes\n";
    std::cout << "Iterations per thread: " << kIterations << "\n\n";

    // ── ANTIPATTERN: false sharing ────────────────────────────────────────────
    SharedCacheLine bad;
    long bad_ms = benchmark_ms([&] {
        std::thread t_a([&] {
            for (long i = 0; i < kIterations; ++i) {
                bad.counter_a.fetch_add(1, std::memory_order_relaxed);
            }
        });
        std::thread t_b([&] {
            for (long i = 0; i < kIterations; ++i) {
                bad.counter_b.fetch_add(1, std::memory_order_relaxed);
            }
        });
        t_a.join();
        t_b.join();
    });

    // ── FIX: padded cache lines ───────────────────────────────────────────────
    PaddedCacheLines good;
    long good_ms = benchmark_ms([&] {
        std::thread t_a([&] {
            for (long i = 0; i < kIterations; ++i) {
                good.counter_a.value.fetch_add(1, std::memory_order_relaxed);
            }
        });
        std::thread t_b([&] {
            for (long i = 0; i < kIterations; ++i) {
                good.counter_b.value.fetch_add(1, std::memory_order_relaxed);
            }
        });
        t_a.join();
        t_b.join();
    });

    std::cout << "False sharing (antipattern): " << bad_ms  << " ms\n";
    std::cout << "Cache-line padded (fix):     " << good_ms << " ms\n";
    if (good_ms > 0) {
        std::cout << "Speedup: " << static_cast<double>(bad_ms) / good_ms << "x\n";
    }
    std::cout << "\nNote: speedup varies by CPU topology, core count, and\n"
              << "OS scheduler. The key insight is the direction, not the\n"
              << "exact multiplier.\n";
    return 0;
}
