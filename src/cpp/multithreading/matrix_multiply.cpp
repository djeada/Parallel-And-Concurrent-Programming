#include <thread>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <vector>

// Function declarations
void sequential_matrix_multiply(long **A, size_t num_rows_a, size_t num_cols_a,
                                long **B, size_t num_rows_b, size_t num_cols_b,
                                long **C);
void parallel_worker(long **A, size_t num_rows_a, size_t num_cols_a,
                     long **B, size_t num_rows_b, size_t num_cols_b,
                     long **C, size_t start_row_c, size_t end_row_c);
void parallel_matrix_multiply(long **A, size_t num_rows_a, size_t num_cols_a,
                              long **B, size_t num_rows_b, size_t num_cols_b,
                              long **C);
void evaluate_performance(int num_eval_runs, size_t num_rows_a, size_t num_cols_a,
                          size_t num_rows_b, size_t num_cols_b);

// Sequential implementation of matrix multiply
void sequential_matrix_multiply(long **A, size_t num_rows_a, size_t num_cols_a,
                                long **B, size_t num_rows_b, size_t num_cols_b,
                                long **C) {
    for (size_t i = 0; i < num_rows_a; i++) {
        for (size_t j = 0; j < num_cols_b; j++) {
            C[i][j] = 0; // initialize result cell to zero
            for (size_t k = 0; k < num_cols_a; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Helper function for parallel matrix multiply
void parallel_worker(long **A, size_t num_rows_a, size_t num_cols_a,
                     long **B, size_t num_rows_b, size_t num_cols_b,
                     long **C, size_t start_row_c, size_t end_row_c) {
    for (size_t i = start_row_c; i < end_row_c; i++) { // subset of rows in A
        for (size_t j = 0; j < num_cols_b; j++) {
            C[i][j] = 0; // initialize result cell to zero
            for (size_t k = 0; k < num_cols_a; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Parallel implementation of matrix multiply
void parallel_matrix_multiply(long **A, size_t num_rows_a, size_t num_cols_a,
                              long **B, size_t num_rows_b, size_t num_cols_b,
                              long **C) {
    size_t num_workers = std::thread::hardware_concurrency();
    size_t chunk_size = ceil((float)num_rows_a / num_workers);

    std::vector<std::thread> workers;
    for (size_t i = 0; i < num_workers; i++) {
        size_t start_row_c = std::min(i * chunk_size, num_rows_a);
        size_t end_row_c = std::min((i + 1) * chunk_size, num_rows_a);
        workers.emplace_back(parallel_worker, A, num_rows_a, num_cols_a,
                             B, num_rows_b, num_cols_b, C, start_row_c, end_row_c);
    }
    for (auto &w : workers) {
        w.join();
    }
}

// Function to evaluate and compare the performance of sequential and parallel implementations
void evaluate_performance(int num_eval_runs, size_t num_rows_a, size_t num_cols_a,
                          size_t num_rows_b, size_t num_cols_b) {
    // Initialize matrices A and B with random values
    long **A = new long*[num_rows_a];
    long **B = new long*[num_rows_b];
    for (size_t i = 0; i < num_rows_a; i++) {
        A[i] = new long[num_cols_a];
        for (size_t j = 0; j < num_cols_a; j++) {
            A[i][j] = rand() % 100 + 1;
        }
    }
    for (size_t i = 0; i < num_rows_b; i++) {
        B[i] = new long[num_cols_b];
        for (size_t j = 0; j < num_cols_b; j++) {
            B[i][j] = rand() % 100 + 1;
        }
    }

    // Allocate arrays for sequential and parallel results
    long **sequential_result = new long*[num_rows_a];
    long **parallel_result = new long*[num_rows_a];
    for (size_t i = 0; i < num_rows_a; i++) {
        sequential_result[i] = new long[num_cols_b];
        parallel_result[i] = new long[num_cols_b];
    }

    std::cout << "Evaluating Sequential Implementation..." << std::endl;
    std::chrono::duration<double> sequential_time(0);
    sequential_matrix_multiply(A, num_rows_a, num_cols_a, B, num_rows_b, num_cols_b, sequential_result); // "warm up"
    for (int i = 0; i < num_eval_runs; i++) {
        auto start_time = std::chrono::high_resolution_clock::now();
        sequential_matrix_multiply(A, num_rows_a, num_cols_a, B, num_rows_b, num_cols_b, sequential_result);
        sequential_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    sequential_time /= num_eval_runs;

    std::cout << "Evaluating Parallel Implementation..." << std::endl;
    std::chrono::duration<double> parallel_time(0);
    parallel_matrix_multiply(A, num_rows_a, num_cols_a, B, num_rows_b, num_cols_b, parallel_result); // "warm up"
    for (int i = 0; i < num_eval_runs; i++) {
        auto start_time = std::chrono::high_resolution_clock::now();
        parallel_matrix_multiply(A, num_rows_a, num_cols_a, B, num_rows_b, num_cols_b, parallel_result);
        parallel_time += std::chrono::high_resolution_clock::now() - start_time;
    }
    parallel_time /= num_eval_runs;

    // Verify sequential and parallel results
    for (size_t i = 0; i < num_rows_a; i++) {
        for (size_t j = 0; j < num_cols_b; j++) {
            if (sequential_result[i][j] != parallel_result[i][j]) {
                std::cerr << "ERROR: Result mismatch at row " << i << ", col " << j << "!" << std::endl;
            }
        }
    }
    std::cout << "Average Sequential Time: " << sequential_time.count() * 1000 << " ms" << std::endl;
    std::cout << "  Average Parallel Time: " << parallel_time.count() * 1000 << " ms" << std::endl;
    std::cout << "Speedup: " << sequential_time / parallel_time << std::endl;
    std::cout << "Efficiency: " << 100 * (sequential_time / parallel_time) / std::thread::hardware_concurrency() << "%" << std::endl;

    // Free allocated memory
    for (size_t i = 0; i < num_rows_a; i++) {
        delete[] A[i];
        delete[] sequential_result[i];
        delete[] parallel_result[i];
    }
    for (size_t i = 0; i < num_rows_b; i++) {
        delete[] B[i];
    }
    delete[] A;
    delete[] B;
    delete[] sequential_result;
    delete[] parallel_result;
}

int main() {
    const int NUM_EVAL_RUNS = 3;
    const size_t NUM_ROWS_A = 10;
    const size_t NUM_COLS_A = 10;
    const size_t NUM_ROWS_B = NUM_COLS_A;
    const size_t NUM_COLS_B = 10;

    evaluate_performance(NUM_EVAL_RUNS, NUM_ROWS_A, NUM_COLS_A, NUM_ROWS_B, NUM_COLS_B);

    return 0;
}
