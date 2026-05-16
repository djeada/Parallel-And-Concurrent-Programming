/**
 * Atomic Memory Ordering
 *
 * C++ atomics support explicit memory ordering to balance correctness
 * and performance. The default (seq_cst) is safe but conservative; weaker
 * orderings can be faster when their guarantees are sufficient.
 *
 * Memory orderings (weakest to strongest):
 * - memory_order_relaxed : atomic operation only; no ordering with other ops
 * - memory_order_release : this STORE is visible before any subsequent acquire
 * - memory_order_acquire : this LOAD sees all stores before the paired release
 * - memory_order_seq_cst : total global order across all seq_cst ops (default)
 *
 * Rule of thumb:
 * - Independent counters → relaxed
 * - Producer/consumer handoff → release on write, acquire on read
 * - Everything else → seq_cst (default) until you have profiling evidence
 *
 * Compile: g++ -std=c++11 -O2 -pthread memory_order.cpp
 */

#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

// ─── Example 1: relaxed — independent counter ─────────────────────────────────
//
// Each thread increments its own slice of work. No ordering relative to
// other threads is needed; only atomicity. After all threads join, the
// accumulated total is correct regardless of ordering.

std::atomic<int> relaxed_counter{0};
constexpr int kIterations = 1000000;

void relaxed_worker() {
    for (int i = 0; i < kIterations; ++i) {
        relaxed_counter.fetch_add(1, std::memory_order_relaxed);
    }
}

void demo_relaxed() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(relaxed_worker);
    for (auto& t : threads) t.join();

    std::cout << "Relaxed counter: " << relaxed_counter.load()
              << "  (expected " << 4 * kIterations << ")\n";
}

// ─── Example 2: acquire/release — safe data handoff ──────────────────────────
//
// Producer writes payload, then RELEASES the flag.
// Consumer ACQUIRES the flag, then reads payload.
// Acquire/release pair ensures the consumer sees the payload write.
//
// Using plain "release" store + "acquire" load is enough here;
// seq_cst would be unnecessarily strong (and slower on some CPUs).

std::atomic<bool> data_ready{false};
int payload = 0;  // NOT atomic, but protected by the acquire/release pair

void producer() {
    payload = 42;                                           // Write data
    data_ready.store(true, std::memory_order_release);      // "Publish" it
}

void consumer() {
    while (!data_ready.load(std::memory_order_acquire)) {}  // Spin-wait
    // Guaranteed to see payload == 42 because of the acquire/release pair
    assert(payload == 42);
    std::cout << "Consumer received payload: " << payload << "\n";
}

void demo_acquire_release() {
    data_ready.store(false, std::memory_order_relaxed);
    payload = 0;
    std::thread prod(producer);
    std::thread cons(consumer);
    prod.join();
    cons.join();
}

// ─── Example 3: seq_cst — the safe default ────────────────────────────────────
//
// operator++ on std::atomic uses seq_cst by default: a total global ordering
// of all seq_cst operations is guaranteed across all threads.
// This is the easiest to reason about and the right first choice.

std::atomic<int> seq_counter{0};

void seq_cst_worker() {
    for (int i = 0; i < kIterations; ++i) {
        ++seq_counter;  // equivalent to fetch_add(1, memory_order_seq_cst)
    }
}

void demo_seq_cst() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(seq_cst_worker);
    for (auto& t : threads) t.join();

    std::cout << "Seq-cst counter:  " << seq_counter.load()
              << "  (expected " << 4 * kIterations << ")\n";
}

int main() {
    std::cout << "=== Relaxed (independent counter) ===\n";
    demo_relaxed();

    std::cout << "\n=== Acquire/Release (data handoff) ===\n";
    demo_acquire_release();

    std::cout << "\n=== Sequential consistency (default) ===\n";
    demo_seq_cst();

    return 0;
}
