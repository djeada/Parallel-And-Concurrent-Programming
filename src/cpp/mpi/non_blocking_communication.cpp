/**
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
 * - Better performance in many scenarios
 *
 * Compile: mpicxx -o non_blocking non_blocking_communication.cpp
 * Run: mpirun -np 2 ./non_blocking
 */

#include <mpi.h>

#include <iostream>

// Simulates useful work during communication
void do_computation(int rank) {
    std::cout << "Process " << rank << ": Performing computation while waiting..."
              << std::endl;
    // Simulate work
    volatile long sum = 0;
    for (long i = 0; i < 1000000; ++i) {
        sum += i;
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            std::cerr << "Error: This program requires at least 2 processes."
                      << std::endl;
        }
        MPI_Finalize();
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
        MPI_Isend(&data, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &request);

        // Do useful work while message is being sent
        do_computation(rank);

        // Wait for send to complete before modifying data
        MPI_Wait(&request, &status);
        std::cout << "Process 0: Send completed" << std::endl;

    } else if (rank == 1) {
        int recv_data = 0;
        std::cout << "Process 1: Initiating non-blocking receive" << std::endl;

        // Start non-blocking receive - returns immediately
        MPI_Irecv(&recv_data, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &request);

        // Do useful work while waiting for message
        do_computation(rank);

        // Wait for receive to complete before using data
        MPI_Wait(&request, &status);
        std::cout << "Process 1: Received value " << recv_data << std::endl;
    }

    MPI_Finalize();
    return 0;
}
