/**
 * Scatter and Gather Operations
 *
 * This example demonstrates MPI_Scatter and MPI_Gather, which are
 * fundamental collective operations for distributing and collecting data.
 *
 * Key concepts:
 * - MPI_Scatter: Root distributes equal chunks of data to all processes
 * - MPI_Gather: Root collects equal chunks of data from all processes
 * - MPI_Scatterv/Gatherv: Variable-size versions for unequal chunks
 *
 * MPI_Scatter parameters:
 *   sendbuf, sendcount, sendtype (at root)
 *   recvbuf, recvcount, recvtype
 *   root, comm
 *
 * MPI_Gather parameters:
 *   sendbuf, sendcount, sendtype
 *   recvbuf, recvcount, recvtype (at root)
 *   root, comm
 *
 * Note: sendcount/recvcount is per-process, not total!
 *
 * Compile: mpicxx -o scatter_gather scatter_gather.cpp
 * Run: mpirun -np 4 ./scatter_gather
 */

#include <mpi.h>

#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    constexpr int elements_per_process = 3;
    constexpr int root = 0;

    // Full data array (only meaningful on root)
    std::vector<int> full_data;

    // Local data buffer for each process
    std::vector<int> local_data(elements_per_process);

    // Root initializes the full data array
    if (rank == root) {
        full_data.resize(size * elements_per_process);
        std::cout << "Root process: Initializing data" << std::endl;
        std::cout << "Full data: ";
        for (int i = 0; i < size * elements_per_process; ++i) {
            full_data[i] = i * 10;  // 0, 10, 20, 30, ...
            std::cout << full_data[i] << " ";
        }
        std::cout << std::endl << std::endl;
    }

    // Scatter: distribute chunks to all processes
    MPI_Scatter(full_data.data(), elements_per_process, MPI_INT,
                local_data.data(), elements_per_process, MPI_INT, root,
                MPI_COMM_WORLD);

    // Each process reports what it received
    std::cout << "Process " << rank << " received: ";
    for (int val : local_data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Each process modifies its data (double each value)
    for (int& val : local_data) {
        val *= 2;
    }

    std::cout << "Process " << rank << " processed: ";
    for (int val : local_data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Gather: collect results back to root
    std::vector<int> result_data;
    if (rank == root) {
        result_data.resize(size * elements_per_process);
    }

    MPI_Gather(local_data.data(), elements_per_process, MPI_INT,
               result_data.data(), elements_per_process, MPI_INT, root,
               MPI_COMM_WORLD);

    // Root prints the final gathered result
    if (rank == root) {
        std::cout << std::endl;
        std::cout << "Root process: Gathered result: ";
        for (int val : result_data) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
