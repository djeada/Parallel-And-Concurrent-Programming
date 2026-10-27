/**
 * C++20 + MPI: scatter/gather uneven row partitions.
 * Scatterv/Gatherv handle ranks with different row counts, including zero rows.
 * Contiguous vectors own data; a span gives the printer a non-owning view.
 * The root verifies the gathered matrix against a sequential reference.
 * Compile: mpicxx -std=c++20 parallel_matrix_multiplication.cpp -o matrix_mult
 * Run: mpiexec -n 3 ./matrix_mult
 */
#include "../common/mpi_support.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <span>
#include <string_view>
#include <vector>

constexpr int N = 4;

void print_matrix(std::string_view name, std::span<const double> matrix) {
    std::cout << name << ":\n";
    for (int row = 0; row < N; ++row) {
        for (int column = 0; column < N; ++column) {
            std::cout << std::setw(8) << std::fixed << std::setprecision(1)
                      << matrix[row * N + column];
        }
        std::cout << '\n';
    }
}

int main(int argc, char* argv[]) {
    return mpi_example::run(argc, argv, [&]() -> int {
        int rank = 0;
        int size = 0;
        mpi_example::check(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
        mpi_example::check(MPI_Comm_size(MPI_COMM_WORLD, &size));

        std::vector<int> counts(size);
        std::vector<int> displacements(size);
        for (int process = 0; process < size; ++process) {
            counts[process] = (N / size + (process < N % size ? 1 : 0)) * N;
        }
        std::exclusive_scan(counts.begin(), counts.end(), displacements.begin(), 0);
        const int local_rows = counts[rank] / N;

        std::vector<double> A(rank == 0 ? N * N : 0);
        std::vector<double> B(N * N);
        std::vector<double> C(rank == 0 ? N * N : 0);
        std::vector<double> local_A(counts[rank]);
        std::vector<double> local_C(counts[rank]);
        if (rank == 0) {
            std::iota(A.begin(), A.end(), 0.0);
            for (int index = 0; index < N * N; ++index) B[index] = index % N;
            print_matrix("Matrix A", A);
            print_matrix("Matrix B", B);
        }

        mpi_example::check(MPI_Scatterv(A.data(), counts.data(), displacements.data(),
            MPI_DOUBLE, local_A.data(), counts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD));
        mpi_example::check(MPI_Bcast(B.data(), N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD));
        for (int row = 0; row < local_rows; ++row) {
            for (int column = 0; column < N; ++column) {
                for (int index = 0; index < N; ++index) {
                    local_C[row * N + column] += local_A[row * N + index] * B[index * N + column];
                }
            }
        }
        mpi_example::check(MPI_Gatherv(local_C.data(), counts[rank], MPI_DOUBLE,
            C.data(), counts.data(), displacements.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD));

        if (rank == 0) {
            std::vector<double> reference(N * N, 0.0);
            for (int row = 0; row < N; ++row) {
                for (int column = 0; column < N; ++column) {
                    for (int index = 0; index < N; ++index) {
                        reference[row * N + column] += A[row * N + index] * B[index * N + column];
                    }
                }
            }
            print_matrix("Result Matrix C = A * B", C);
            const bool valid = C == reference; // These small integer-valued doubles are exact.
            std::cout << "Matches sequential reference: " << std::boolalpha << valid << '\n';
            if (!valid) return 1;
        }
        return 0;
    });
}
