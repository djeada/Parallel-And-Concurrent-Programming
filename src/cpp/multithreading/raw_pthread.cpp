/**
 * POSIX Threads (pthreads)
 *
 * This example demonstrates the low-level pthread API for comparison
 * with modern C++ threading. While std::thread is preferred in modern C++,
 * understanding pthreads helps when working with legacy code or specific
 * platforms.
 *
 * Key concepts:
 * - pthread_create() starts a new thread
 * - pthread_join() waits for thread completion
 * - Thread functions take and return void*
 * - Data is passed via struct pointers
 *
 * Note: Modern C++ code should prefer std::thread for portability
 * and type safety.
 */

#include <iostream>
#include <pthread.h>
#include <vector>

// Data structure to pass to thread and receive results
struct SumData {
    long long start;
    long long end;
    long long result;
};

// Thread function - must have signature: void* func(void*)
void* compute_sum(void* data) {
    auto* sum_data = static_cast<SumData*>(data);

    sum_data->result = 0;
    for (long long i = sum_data->start; i < sum_data->end; ++i) {
        sum_data->result += i;
    }

    return nullptr;
}

int main() {
    constexpr int num_threads = 10;
    constexpr long long range_start = 0;
    constexpr long long range_end = 1'000'000;

    std::vector<pthread_t> threads(num_threads);
    std::vector<SumData> thread_data(num_threads);

    // Divide work among threads
    long long chunk_size = (range_end - range_start) / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = range_start + chunk_size * i;
        thread_data[i].end = range_start + chunk_size * (i + 1);
        thread_data[i].result = 0;

        pthread_create(&threads[i], nullptr, compute_sum, &thread_data[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Combine results
    long long total = 0;
    for (int i = 0; i < num_threads; ++i) {
        total += thread_data[i].result;
    }

    std::cout << "Sum of 0 to " << (range_end - 1) << " = " << total << "\n";
    std::cout << "Expected: " << ((range_end - 1) * range_end / 2) << "\n";

    return 0;
}
