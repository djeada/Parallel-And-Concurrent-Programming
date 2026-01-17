/**
 * Parallel Matrix Multiplication
 *
 * This example demonstrates parallel matrix multiplication using
 * MPI_Scatter and MPI_Gather. Each process computes a portion of
 * the result matrix.
 *
 * Key concepts:
 * - MPI_Scatter: Distributes data from root to all processes
 * - MPI_Gather: Collects data from all processes to root
 * - MPI_Bcast: Broadcasts the entire B matrix to all processes
 * - Data decomposition: Each process handles rows_per_process rows of A
 *
 * Algorithm:
 * 1. Root initializes matrices A and B
 * 2. Scatter rows of A to all processes
 * 3. Broadcast entire B to all processes
 * 4. Each process computes its portion of C = A * B
 * 5. Gather results back to root
 *
 * Note: This assumes matrix size N is evenly divisible by process count.
 *
 * Compile: mpicxx -o matrix_mult parallel_matrix_multiplication.cpp
 * Run: mpirun -np 4 ./matrix_mult
 */

#include <mpi.h>

#include <iomanip>
#include <iostream>
#include <vector>

constexpr int N = 4;  // Matrix size (N x N)

void print_matrix(const std::string& name, const std::vector<double>& matrix) {
    std::cout << name << ":" << std::endl;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            std::cout << std::setw(8) << std::fixed << std::setprecision(1)
                      << matrix[i * N + j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check that N is divisible by process count
    if (N % size != 0) {
        if (rank == 0) {
            std::cerr << "Error: Matrix size " << N
                      << " must be divisible by process count " << size
                      << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    int rows_per_process = N / size;

    // Full matrices (only meaningful on root)
    std::vector<double> A(N * N);
    std::vector<double> B(N * N);
    std::vector<double> C(N * N, 0.0);

    // Local portions
    std::vector<double> local_A(rows_per_process * N);
    std::vector<double> local_C(rows_per_process * N, 0.0);

    // Root initializes matrices
    if (rank == 0) {
        std::cout << "Parallel Matrix Multiplication (" << N << "x" << N
                  << ") with " << size << " processes" << std::endl;
        std::cout << "Each process handles " << rows_per_process << " rows"
                  << std::endl;
        std::cout << std::endl;

        // Initialize A and B with simple values
        for (int i = 0; i < N * N; ++i) {
            A[i] = static_cast<double>(i);
            B[i] = static_cast<double>(i % N);
        }

        print_matrix("Matrix A", A);
        print_matrix("Matrix B", B);
    }

    // Scatter rows of A to all processes
    MPI_Scatter(A.data(), rows_per_process * N, MPI_DOUBLE, local_A.data(),
                rows_per_process * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Broadcast entire B to all processes
    MPI_Bcast(B.data(), N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Each process computes its portion of C
    for (int i = 0; i < rows_per_process; ++i) {
        for (int j = 0; j < N; ++j) {
            double sum = 0.0;
            for (int k = 0; k < N; ++k) {
                sum += local_A[i * N + k] * B[k * N + j];
            }
            local_C[i * N + j] = sum;
        }
    }

    std::cout << "Process " << rank << ": Computed " << rows_per_process
              << " rows of result" << std::endl;

    // Gather results back to root
    MPI_Gather(local_C.data(), rows_per_process * N, MPI_DOUBLE, C.data(),
               rows_per_process * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Root prints the result
    if (rank == 0) {
        std::cout << std::endl;
        print_matrix("Result Matrix C = A * B", C);
    }

    MPI_Finalize();
    return 0;
}
