/**
 * Reduce Operation
 *
 * This example demonstrates MPI_Reduce, a collective operation that combines
 * values from all processes using an operation (sum, max, min, etc.) and
 * returns the result to one process (the root).
 *
 * Key concepts:
 * - MPI_Reduce: All-to-one collective operation with computation
 * - Reduction operations: MPI_SUM, MPI_MAX, MPI_MIN, MPI_PROD, etc.
 * - Only root receives the final result
 * - All processes must participate in the collective call
 *
 * Common reduction operations:
 * - MPI_SUM: Sum of all values
 * - MPI_PROD: Product of all values
 * - MPI_MAX: Maximum value
 * - MPI_MIN: Minimum value
 * - MPI_LAND: Logical AND
 * - MPI_LOR: Logical OR
 *
 * Compile: mpicxx -o reduce reduce_operation.cpp
 * Run: mpirun -np 4 ./reduce
 *
 * With 4 processes, sum = 1+2+3+4 = 10, max = 4, min = 1
 */

#include <mpi.h>

#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Each process has a local value
    int local_value = rank + 1;
    std::cout << "Process " << rank << ": local value = " << local_value
              << std::endl;

    constexpr int root = 0;

    // Reduce with SUM operation
    int global_sum = 0;
    MPI_Reduce(&local_value, &global_sum, 1, MPI_INT, MPI_SUM, root,
               MPI_COMM_WORLD);

    // Reduce with MAX operation
    int global_max = 0;
    MPI_Reduce(&local_value, &global_max, 1, MPI_INT, MPI_MAX, root,
               MPI_COMM_WORLD);

    // Reduce with MIN operation
    int global_min = 0;
    MPI_Reduce(&local_value, &global_min, 1, MPI_INT, MPI_MIN, root,
               MPI_COMM_WORLD);

    // Only root has the final results
    if (rank == root) {
        std::cout << "\n=== Reduction Results ===" << std::endl;
        std::cout << "Sum of all values: " << global_sum << std::endl;
        std::cout << "Maximum value: " << global_max << std::endl;
        std::cout << "Minimum value: " << global_min << std::endl;
    }

    MPI_Finalize();
    return 0;
}
