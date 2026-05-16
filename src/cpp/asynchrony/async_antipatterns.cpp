/**
 * ⚠️  ANTIPATTERN — Common std::async / std::future Pitfalls
 *
 * This file demonstrates three bugs that surprise C++ developers working
 * with std::async for the first time.  Each demo is clearly labelled.
 * The fixes are shown inline.
 *
 * Pitfall 1 — The "forgotten future" serialises a loop
 *   std::async(std::launch::async, ...) returns a future whose destructor
 *   BLOCKS until the task finishes — but only for futures created by
 *   std::async (not packaged_task or promise).
 *   If you don't store the future, it is a temporary and is destroyed at
 *   the end of the statement, serialising what was meant to be parallel work.
 *
 * Pitfall 2 — Default launch policy is implementation-defined
 *   std::async(f) without an explicit policy may run f deferred (in the
 *   caller's thread on .get()) or in a new thread — the implementation
 *   decides.  Always specify std::launch::async or std::launch::deferred.
 *   Use wait_for(0s) == future_status::deferred to detect deferred tasks.
 *
 * Pitfall 3 — Calling .get() twice throws std::future_error
 *   After the first .get() the future's shared state is invalidated.
 *   Use future.valid() to check before calling .get().
 *
 * Compile:
 *   g++ -std=c++17 -pthread -O1 async_antipatterns.cpp
 */

#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

// ── Pitfall 1: forgotten future serialises the loop ────────────────────────

void demo_forgotten_future() {
    std::cout << "=== ANTIPATTERN 1: forgotten future serialises a loop ===\n";

    auto measure = [](bool store_futures) {
        auto t0 = std::chrono::steady_clock::now();

        if (store_futures) {
            // CORRECT: store futures so tasks run in parallel
            std::vector<std::future<void>> futs;
            for (int i = 0; i < 4; ++i) {
                futs.push_back(std::async(std::launch::async, []() {
                    std::this_thread::sleep_for(200ms);
                }));
            }
            for (auto& f : futs) f.get();
        } else {
            // ⚠️ ANTIPATTERN: temporary future destroyed immediately — each
            // iteration waits for the previous task before starting the next.
            // Note: GCC/Clang emit a -Wunused-result warning here because
            // std::async is [[nodiscard]] — the compiler itself flags this bug.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
            for (int i = 0; i < 4; ++i) {
                std::async(std::launch::async, []() {  // result discarded!
                    std::this_thread::sleep_for(200ms);
                });
                // temporary's destructor fires here → blocks ~200 ms
            }
#pragma GCC diagnostic pop
        }

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - t0).count();
        return ms;
    };

    long antipattern_ms = measure(false);
    long correct_ms     = measure(true);

    std::cout << "  ⚠️  antipattern (forgotten futures): " << antipattern_ms << " ms  "
              << "(tasks ran sequentially)\n";
    std::cout << "  ✓  correct     (stored futures):     " << correct_ms << " ms  "
              << "(tasks ran in parallel)\n\n";
}

// ── Pitfall 2: default launch policy is implementation-defined ─────────────

void demo_default_launch_policy() {
    std::cout << "=== ANTIPATTERN 2: ambiguous default launch policy ===\n";

    // Using default policy — may be async OR deferred
    auto fut_default  = std::async([]{ return 1; });
    // Explicitly deferred — always runs in caller on .get()
    auto fut_deferred = std::async(std::launch::deferred, []{ return 2; });
    // Explicitly async — always a new thread
    auto fut_async    = std::async(std::launch::async, []{ return 3; });

    // Detect deferred status without blocking
    auto check = [](std::future<int>& f, const char* label) {
        auto s = f.wait_for(0ms);
        if (s == std::future_status::deferred)
            std::cout << "  " << label << ": deferred (will run on .get())\n";
        else if (s == std::future_status::ready)
            std::cout << "  " << label << ": already ready\n";
        else
            std::cout << "  " << label << ": running in background thread\n";
    };

    check(fut_default,  "default policy ");
    check(fut_deferred, "launch::deferred");
    check(fut_async,    "launch::async  ");

    std::cout << "  → Always use an explicit policy to avoid surprises.\n\n";

    fut_default.get();
    fut_deferred.get();
    fut_async.get();
}

// ── Pitfall 3: calling .get() twice throws std::future_error ───────────────

void demo_double_get() {
    std::cout << "=== ANTIPATTERN 3: double .get() throws ===\n";

    auto fut = std::async(std::launch::async, []{ return 42; });

    int first = fut.get();
    std::cout << "  First .get() = " << first << "\n";

    // ⚠️ ANTIPATTERN: future is now invalid after .get()
    std::cout << "  fut.valid() after .get() = " << std::boolalpha << fut.valid() << "\n";

    try {
        int second = fut.get();  // throws std::future_error
        (void)second;
    } catch (const std::future_error& e) {
        std::cout << "  ⚠️  Second .get() threw std::future_error: "
                  << e.what() << "\n";
    }

    // FIX: check valid() before calling .get()
    std::cout << "  → Guard with: if (fut.valid()) fut.get();\n";
    std::cout << "  → Or use std::shared_future to allow multiple .get() calls.\n\n";
}

int main() {
    demo_forgotten_future();
    demo_default_launch_policy();
    demo_double_get();
    return 0;
}
