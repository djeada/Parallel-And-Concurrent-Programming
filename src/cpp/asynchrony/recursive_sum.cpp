#include <cstdio>
#include <future>

unsigned long long calculate_sum(unsigned int lo, unsigned int hi, unsigned int depth = 0) {
    if (depth > 3) { // base case threshold
        unsigned long long sum = 0;
        for (auto i = lo; i < hi; i++) {
            sum += i;
        }
        return sum;
    } else {  // divide and conquer
        auto mid = (hi + lo) / 2; // middle index for splitting
        auto left = std::async(std::launch::async, calculate_sum, lo, mid, depth + 1);
        auto right = calculate_sum(mid, hi, depth + 1);
        return left.get() + right;
    }
}

int main() {
    unsigned long long total = calculate_sum(0, 1000000000);
    printf("Total: %llu\n", total);
    return 0;
}
