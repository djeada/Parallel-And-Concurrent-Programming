/**
 * Point-to-Point Communication
 *
 * This example demonstrates basic point-to-point communication using
 * MPI_Send and MPI_Recv. These are blocking operations - the sender
 * waits until the message is safely buffered, and the receiver waits
 * until the message arrives.
 *
 * Key concepts:
 * - MPI_Send: Blocking send operation
 * - MPI_Recv: Blocking receive operation
 * - Message envelope: (source, destination, tag, communicator)
 * - MPI_Status: Contains info about received message
 * - MPI_STATUS_IGNORE: Use when status info is not needed
 *
 * Parameters for MPI_Send:
 *   buffer, count, datatype, destination, tag, communicator
 *
 * Parameters for MPI_Recv:
 *   buffer, count, datatype, source, tag, communicator, status
 *
 * Compile: mpicxx -o point_to_point point_to_point_communication.cpp
 * Run: mpirun -np 2 ./point_to_point
 */

#include <mpi.h>

#include <iostream>

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

    constexpr int tag = 0;

    if (rank == 0) {
        // Process 0 sends data to process 1
        int data = 42;
        std::cout << "Process 0: Sending value " << data << " to process 1"
                  << std::endl;

        MPI_Send(&data, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);

        std::cout << "Process 0: Message sent successfully" << std::endl;

    } else if (rank == 1) {
        // Process 1 receives data from process 0
        int received_data = 0;

        MPI_Recv(&received_data, 1, MPI_INT, 0, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        std::cout << "Process 1: Received value " << received_data
                  << " from process 0" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
