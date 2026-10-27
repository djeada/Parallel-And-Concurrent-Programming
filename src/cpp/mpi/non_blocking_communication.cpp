/**
 * C++20 + MPI: scoped environment and checked calls through the supported C API.
 *
 * Non-Blocking Communication
 *
 * This example demonstrates non-blocking communication using MPI_Isend
 * and MPI_Irecv. Unlike blocking operations, these return immediately,
 * allowing computation to overlap with communication.
 *
 * Key concepts:
 * - MPI_Isend: Initiates a non-blocking send, returns immediately
 * - MPI_Irecv: Initiates a non-blocking receive, returns immediately
 * - MPI_Request: Handle to track the status of the operation
 * - MPI_Wait: Blocks until the operation completes
 * - MPI_Test: Checks if operation completed (non-blocking check)
 * - MPI_Waitall: Waits for multiple operations to complete
 *
 * Benefits of non-blocking communication:
 * - Overlap computation with communication
 * - Avoid deadlocks in complex communication patterns
 * - May overlap work and transfer; actual progress depends on the MPI runtime
 *
 * Compile: mpicxx -std=c++20 -o non_blocking non_blocking_communication.cpp
 * Run: mpirun -np 2 ./non_blocking
 */

#include "../common/mpi_support.hpp"

#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

// Simulates useful work during communication
void do_computation(int rank) {
    std::cout << "Process " << rank << ": Performing computation while waiting..."
              << std::endl;
    // Keep the computed result observable instead of using volatile busy work.
    std::vector<int> values(10'000);
    std::iota(values.begin(), values.end(), 0);
    const auto sum = std::accumulate(values.begin(), values.end(), std::int64_t{0});
    std::cout << "Process " << rank << ": local checksum = " << sum << '\n';
}

int main(int argc, char* argv[]) {
    return mpi_example::run(argc, argv, [&]() -> int {

        int rank = 0;
        int size = 0;
        mpi_example::check(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
        mpi_example::check(MPI_Comm_size(MPI_COMM_WORLD, &size));

        if (size < 2) {
            if (rank == 0) {
                std::cerr << "Error: This program requires at least 2 processes."
                          << std::endl;
            }
            return 1;
        }

        MPI_Request request;
        MPI_Status status;
        constexpr int tag = 0;

        if (rank == 0) {
            int data = 100;
            std::cout << "Process 0: Initiating non-blocking send of value "
                      << data << std::endl;

            // Start non-blocking send - returns immediately
            mpi_example::check(MPI_Isend(&data, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &request));

            // Do useful work while message is being sent
            do_computation(rank);

            // Wait for send to complete before modifying data
            mpi_example::check(MPI_Wait(&request, &status));
            std::cout << "Process 0: Send completed" << std::endl;

        } else if (rank == 1) {
            int recv_data = 0;
            std::cout << "Process 1: Initiating non-blocking receive" << std::endl;

            // Start non-blocking receive - returns immediately
            mpi_example::check(MPI_Irecv(&recv_data, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &request));

            // Do useful work while waiting for message
            do_computation(rank);

            // Wait for receive to complete before using data
            mpi_example::check(MPI_Wait(&request, &status));
            std::cout << "Process 1: Received value " << recv_data << std::endl;
        }
        return 0;
    });
}
