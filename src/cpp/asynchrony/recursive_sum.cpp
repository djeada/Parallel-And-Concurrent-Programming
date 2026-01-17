/**
 * Parallel Recursive Sum with Async
 *
 * This example demonstrates divide-and-conquer parallelism using
 * std::async for computing the sum of a range of numbers.
 *
 * Key concepts:
 * - Divide problem into subproblems, solve in parallel
 * - Depth limiting prevents excessive thread creation
 * - Beyond threshold, use sequential computation
 * - Combine results from async tasks
 *
 * This pattern is applicable to many recursive algorithms:
 * merge sort, quicksort, tree traversal, etc.
 */

#include <future>
#include <iostream>

unsigned long long parallel_sum(unsigned int lo, unsigned int hi, int depth = 0) {
    // Depth limit: prevent too many threads
    constexpr int max_depth = 4;

    if (depth >= max_depth || hi - lo < 1000) {
        // Base case: compute sequentially
        unsigned long long sum = 0;
        for (unsigned int i = lo; i < hi; ++i) {
            sum += i;
        }
        return sum;
    }

    // Divide and conquer
    unsigned int mid = lo + (hi - lo) / 2;

    // Left half in async task
    auto left = std::async(std::launch::async, parallel_sum, lo, mid, depth + 1);

    // Right half in current thread
    unsigned long long right = parallel_sum(mid, hi, depth + 1);

    // Combine results
    return left.get() + right;
}

int main() {
    constexpr unsigned int N = 1'000'000'000;

    std::cout << "Computing sum of 0 to " << (N - 1) << "...\n";

    unsigned long long result = parallel_sum(0, N);

    std::cout << "Result: " << result << "\n";
    std::cout << "Expected: " << ((unsigned long long)(N - 1) * N / 2) << "\n";

    return 0;
}
