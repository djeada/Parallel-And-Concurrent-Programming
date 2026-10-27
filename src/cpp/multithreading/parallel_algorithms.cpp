/**
 * Modern C++20 Learning Path: Prefer an Algorithm for Data Parallelism
 *
 * C++17 introduced execution policies; they remain the starting point here
 * for independent reductions and sorting. The library manages partitioning.
 * std::execution::par permits parallelism but may execute sequentially.
 * Callbacks must avoid shared mutable state; integer arithmetic here is exact
 * and stays in range. Floating-point reductions can change rounding order.
 *
 * Compile: g++ -std=c++20 -O2 -pthread parallel_algorithms.cpp
 * With libstdc++'s TBB backend, add -ltbb (and install TBB development headers).
 */

#include <algorithm>
#include <cstdint>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::vector<int> values(1'000'000);
    std::iota(values.begin(), values.end(), 0);
    std::reverse(values.begin(), values.end());

    const auto expected = std::accumulate(values.begin(), values.end(), std::int64_t{0});
    const auto sum = std::transform_reduce(
        std::execution::par, values.begin(), values.end(), std::int64_t{0},
        std::plus<>{}, [](int value) noexcept { return std::int64_t{value}; });

    std::sort(std::execution::par, values.begin(), values.end());
    const bool sorted = std::is_sorted(values.begin(), values.end());
    std::cout << "Sum: " << sum << '\n'
              << "Expected: " << expected << '\n'
              << "Sorted: " << std::boolalpha << sorted << '\n';

    return sum == expected && sorted ? 0 : 1;
}
