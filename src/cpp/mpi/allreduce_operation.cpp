/**
 * Allreduce Operation
 *
 * This example demonstrates MPI_Allreduce, which performs a reduction
 * operation and broadcasts the result to ALL processes (unlike MPI_Reduce
 * which only sends the result to the root).
 *
 * Key concepts:
 * - MPI_Allreduce: Combines values from all processes, result to all
 * - Equivalent to MPI_Reduce + MPI_Bcast but more efficient
 * - All processes have the same result after the call
 *
 * Use cases:
 * - Global convergence checks in iterative algorithms
 * - Computing global statistics needed by all processes
 * - Normalization factors in parallel algorithms
 *
 * Common pattern in iterative solvers:
 *   while (global_error > tolerance) {
 *       compute_local_error();
 *       MPI_Allreduce(&local_error, &global_error, 1, MPI_DOUBLE, MPI_MAX, comm);
 *   }
 *
 * Compile: mpicxx -o allreduce allreduce_operation.cpp
 * Run: mpirun -np 4 ./allreduce
 */

#include <mpi.h>

#include <cmath>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Each process has local data
    double local_value = static_cast<double>(rank + 1);
    std::cout << "Process " << rank << ": local value = " << local_value
              << std::endl;

    // Allreduce with SUM - all processes get the sum
    double global_sum = 0.0;
    MPI_Allreduce(&local_value, &global_sum, 1, MPI_DOUBLE, MPI_SUM,
                  MPI_COMM_WORLD);

    std::cout << "Process " << rank << ": global sum = " << global_sum
              << std::endl;

    // Compute local contribution as percentage of total
    double local_percentage = (local_value / global_sum) * 100.0;
    std::cout << "Process " << rank << ": contributes "
              << local_percentage << "% of total" << std::endl;

    // Demonstrate with array: compute global mean
    std::vector<double> local_data = {
        static_cast<double>(rank * 10 + 1),
        static_cast<double>(rank * 10 + 2),
        static_cast<double>(rank * 10 + 3)
    };

    // Compute local sum
    double local_sum = 0.0;
    for (double val : local_data) {
        local_sum += val;
    }

    // Get global sum and compute mean
    double total_sum = 0.0;
    MPI_Allreduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM,
                  MPI_COMM_WORLD);

    int total_elements = static_cast<int>(local_data.size()) * size;
    double global_mean = total_sum / total_elements;

    // All processes now have the global mean
    std::cout << "Process " << rank << ": global mean = " << global_mean
              << std::endl;

    // Use global mean to compute local variance contribution
    double local_variance_sum = 0.0;
    for (double val : local_data) {
        double diff = val - global_mean;
        local_variance_sum += diff * diff;
    }

    // Get global variance
    double global_variance_sum = 0.0;
    MPI_Allreduce(&local_variance_sum, &global_variance_sum, 1, MPI_DOUBLE,
                  MPI_SUM, MPI_COMM_WORLD);

    double global_std_dev = std::sqrt(global_variance_sum / total_elements);

    if (rank == 0) {
        std::cout << std::endl;
        std::cout << "=== Global Statistics ===" << std::endl;
        std::cout << "Total sum: " << total_sum << std::endl;
        std::cout << "Mean: " << global_mean << std::endl;
        std::cout << "Standard deviation: " << global_std_dev << std::endl;
    }

    MPI_Finalize();
    return 0;
}
