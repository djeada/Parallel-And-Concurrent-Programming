/**
 * Parallel Merge Sort
 *
 * This example demonstrates divide-and-conquer parallelism using merge sort.
 * The algorithm splits work recursively, with each thread handling a subtree.
 *
 * Key concepts:
 * - Divide-and-conquer parallelism: split problem, solve in parallel, combine
 * - Depth-limited parallelism: stop spawning threads beyond a threshold
 * - Thread overhead: too many threads can hurt performance
 * - Compare parallel vs sequential to see actual speedup
 *
 * The depth limit prevents creating too many threads. Beyond a certain depth,
 * sequential execution is more efficient due to thread creation overhead.
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

// Merge two sorted subarrays: [left..mid] and [mid+1..right]
void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    std::copy(temp.begin(), temp.end(), arr.begin() + left);
}

// Sequential merge sort
void sequential_sort(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        sequential_sort(arr, left, mid);
        sequential_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Parallel merge sort with depth limiting
void parallel_sort(std::vector<int>& arr, int left, int right, int depth = 0) {
    // Stop spawning threads beyond log2(num_cores) depth
    // Handle case where hardware_concurrency() returns 0
    unsigned int cores = std::thread::hardware_concurrency();
    if (cores == 0) cores = 2;  // Default to 2 if unknown
    int max_depth = static_cast<int>(std::log2(cores));

    if (left >= right) return;

    int mid = left + (right - left) / 2;

    if (depth < max_depth) {
        // Spawn thread for left half, do right half ourselves
        std::thread left_thread(parallel_sort, std::ref(arr),
                                left, mid, depth + 1);
        parallel_sort(arr, mid + 1, right, depth + 1);
        left_thread.join();
    } else {
        // Beyond depth limit, use sequential sort
        sequential_sort(arr, left, mid);
        sequential_sort(arr, mid + 1, right);
    }

    merge(arr, left, mid, right);
}

void benchmark(int size, int runs) {
    // Initialize array with random values
    std::vector<int> original(size);
    for (int i = 0; i < size; ++i) {
        original[i] = rand();
    }

    using namespace std::chrono;

    // Benchmark sequential
    std::cout << "Evaluating Sequential Merge Sort...\n";
    duration<double> seq_time(0);
    std::vector<int> seq_arr = original;
    sequential_sort(seq_arr, 0, size - 1);

    for (int r = 0; r < runs; ++r) {
        std::vector<int> arr = original;
        auto start = high_resolution_clock::now();
        sequential_sort(arr, 0, size - 1);
        seq_time += high_resolution_clock::now() - start;
    }
    seq_time /= runs;

    // Benchmark parallel
    std::cout << "Evaluating Parallel Merge Sort...\n";
    duration<double> par_time(0);
    std::vector<int> par_arr = original;
    parallel_sort(par_arr, 0, size - 1);

    for (int r = 0; r < runs; ++r) {
        std::vector<int> arr = original;
        auto start = high_resolution_clock::now();
        parallel_sort(arr, 0, size - 1);
        par_time += high_resolution_clock::now() - start;
    }
    par_time /= runs;

    // Verify results
    if (seq_arr != par_arr) {
        std::cerr << "ERROR: Results don't match!\n";
        return;
    }

    std::cout << "\nResults (" << size << " elements):\n";
    std::cout << "  Sequential: " << seq_time.count() * 1000 << " ms\n";
    std::cout << "  Parallel:   " << par_time.count() * 1000 << " ms\n";
    std::cout << "  Speedup:    " << seq_time / par_time << "x\n";
}

int main() {
    benchmark(100000, 5);
    return 0;
}
