/**
 * Parallel Computation of Pi
 *
 * This example demonstrates a classic parallel computing problem:
 * computing Pi using numerical integration of the arctangent derivative.
 * Each process computes a portion of the sum, and results are reduced.
 *
 * Mathematical basis:
 *   pi = integral from 0 to 1 of 4/(1+x^2) dx
 *
 * Key concepts:
 * - Work distribution: Each process handles every size-th interval
 * - MPI_Reduce: Combines partial sums from all processes
 * - Midpoint rule: Uses (i + 0.5) * h for better accuracy
 *
 * This is an embarrassingly parallel problem - no communication
 * between processes during computation, only at the end for reduction.
 *
 * Compile: mpicxx -o compute_pi parallel_computation_of_pi.cpp
 * Run: mpirun -np 4 ./compute_pi
 *
 * Expected output: Pi ≈ 3.14159265358979...
 */

#include <mpi.h>

#include <cmath>
#include <iomanip>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Number of intervals for numerical integration
    constexpr long n = 100000000;  // 100 million intervals
    const double h = 1.0 / static_cast<double>(n);

    // Each process computes its portion of the sum
    // Process i handles intervals: i, i+size, i+2*size, ...
    double local_sum = 0.0;
    for (long i = rank; i < n; i += size) {
        double x = h * (static_cast<double>(i) + 0.5);  // Midpoint
        local_sum += 4.0 / (1.0 + x * x);
    }
    local_sum *= h;

    // Report local contribution
    std::cout << "Process " << rank << ": local_sum = " << std::fixed
              << std::setprecision(10) << local_sum << std::endl;

    // Reduce all local sums to get pi
    double pi = 0.0;
    MPI_Reduce(&local_sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Root prints the result
    if (rank == 0) {
        std::cout << std::endl;
        std::cout << "=== Results ===" << std::endl;
        std::cout << "Computed Pi: " << std::setprecision(15) << pi
                  << std::endl;
        std::cout << "Reference:   " << M_PI << std::endl;
        std::cout << "Error:       " << std::scientific << std::fabs(pi - M_PI)
                  << std::endl;
        std::cout << "Intervals:   " << n << std::endl;
        std::cout << "Processes:   " << size << std::endl;
    }

    MPI_Finalize();
    return 0;
}
