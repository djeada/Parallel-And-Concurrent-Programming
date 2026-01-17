/**
 * Broadcast Communication
 *
 * This example demonstrates MPI_Bcast, a collective operation that sends
 * data from one process (the root) to all other processes in a communicator.
 * Broadcast is essential for distributing configuration, parameters, or
 * initial data to all processes.
 *
 * Key concepts:
 * - MPI_Bcast: One-to-all collective communication
 * - Root process: The process that sends the data
 * - All processes must call MPI_Bcast (collective operation)
 * - After the call, all processes have the same data
 *
 * Parameters for MPI_Bcast:
 *   buffer, count, datatype, root, communicator
 *
 * Compile: mpicxx -o broadcast broadcast_communication.cpp
 * Run: mpirun -np 4 ./broadcast
 */

#include <mpi.h>

#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Data buffer - root initializes, others receive
    int data = 0;
    constexpr int root = 0;

    if (rank == root) {
        // Root process sets the data to broadcast
        data = 42;
        std::cout << "Process " << rank << " (root): Broadcasting value "
                  << data << " to all processes" << std::endl;
    } else {
        std::cout << "Process " << rank << ": Waiting to receive broadcast..."
                  << std::endl;
    }

    // All processes call MPI_Bcast - this is a collective operation
    MPI_Bcast(&data, 1, MPI_INT, root, MPI_COMM_WORLD);

    // After broadcast, all processes have the same data
    std::cout << "Process " << rank << ": Received value " << data
              << std::endl;

    MPI_Finalize();
    return 0;
}
