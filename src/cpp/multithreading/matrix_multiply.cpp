/**
 * Parallel Matrix Multiplication
 *
 * This example demonstrates data parallelism by distributing matrix
 * multiplication across multiple threads. Each thread computes a
 * subset of rows in the result matrix.
 *
 * Key concepts:
 * - Data parallelism: same operation on different data partitions
 * - Work distribution: divide rows among available threads
 * - No synchronization needed - each thread writes to different locations
 * - Speedup depends on problem size and number of cores
 *
 * This is a compute-bound task that benefits from parallel execution.
 */

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

using Matrix = std::vector<std::vector<long>>;

// Sequential matrix multiplication
void sequential_multiply(const Matrix& A, const Matrix& B, Matrix& C) {
    size_t rows = A.size();
    size_t cols = B[0].size();
    size_t inner = A[0].size();

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            C[i][j] = 0;
            for (size_t k = 0; k < inner; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Worker function: multiply rows [start_row, end_row) of A with B
void parallel_worker(const Matrix& A, const Matrix& B, Matrix& C,
                     size_t start_row, size_t end_row) {
    size_t cols = B[0].size();
    size_t inner = A[0].size();

    for (size_t i = start_row; i < end_row; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            C[i][j] = 0;
            for (size_t k = 0; k < inner; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Parallel matrix multiplication
void parallel_multiply(const Matrix& A, const Matrix& B, Matrix& C) {
    size_t num_threads = std::thread::hardware_concurrency();
    size_t rows = A.size();
    size_t chunk_size = static_cast<size_t>(std::ceil(
        static_cast<double>(rows) / num_threads));

    std::vector<std::thread> workers;
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = std::min(i * chunk_size, rows);
        size_t end = std::min((i + 1) * chunk_size, rows);

        if (start < end) {
            workers.emplace_back(parallel_worker,
                                 std::cref(A), std::cref(B), std::ref(C),
                                 start, end);
        }
    }

    for (auto& w : workers) {
        w.join();
    }
}

void benchmark(size_t size, int runs) {
    // Initialize matrices with random values
    Matrix A(size, std::vector<long>(size));
    Matrix B(size, std::vector<long>(size));
    Matrix seq_result(size, std::vector<long>(size));
    Matrix par_result(size, std::vector<long>(size));

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            A[i][j] = rand() % 100 + 1;
            B[i][j] = rand() % 100 + 1;
        }
    }

    using namespace std::chrono;

    // Benchmark sequential
    std::cout << "Evaluating Sequential...\n";
    duration<double> seq_time(0);
    sequential_multiply(A, B, seq_result);

    for (int r = 0; r < runs; ++r) {
        auto start = high_resolution_clock::now();
        sequential_multiply(A, B, seq_result);
        seq_time += high_resolution_clock::now() - start;
    }
    seq_time /= runs;

    // Benchmark parallel
    std::cout << "Evaluating Parallel...\n";
    duration<double> par_time(0);
    parallel_multiply(A, B, par_result);

    for (int r = 0; r < runs; ++r) {
        auto start = high_resolution_clock::now();
        parallel_multiply(A, B, par_result);
        par_time += high_resolution_clock::now() - start;
    }
    par_time /= runs;

    // Verify results match
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            if (seq_result[i][j] != par_result[i][j]) {
                std::cerr << "ERROR: Mismatch at [" << i << "][" << j << "]\n";
                return;
            }
        }
    }

    std::cout << "\nResults (" << size << "x" << size << " matrix):\n";
    std::cout << "  Sequential: " << seq_time.count() * 1000 << " ms\n";
    std::cout << "  Parallel:   " << par_time.count() * 1000 << " ms\n";
    std::cout << "  Speedup:    " << seq_time / par_time << "x\n";
}

int main() {
    benchmark(200, 3);
    return 0;
}
