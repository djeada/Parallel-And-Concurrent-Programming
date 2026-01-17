/**
 * Asynchronous Communication with MPI_Probe
 *
 * This example demonstrates how to receive messages of unknown size using
 * MPI_Probe. This is useful when the receiver doesn't know the message
 * size in advance.
 *
 * Key concepts:
 * - MPI_Probe: Blocking check for incoming message (doesn't receive it)
 * - MPI_Iprobe: Non-blocking check for incoming message
 * - MPI_Get_count: Gets the number of elements in a message
 * - MPI_Status: Contains source, tag, and error information
 *
 * Workflow:
 * 1. Use MPI_Probe to check for a message and get its metadata
 * 2. Use MPI_Get_count to determine the message size
 * 3. Allocate appropriate buffer
 * 4. Use MPI_Recv to actually receive the message
 *
 * This pattern is essential when message sizes vary dynamically.
 *
 * Compile: mpicxx -o async_probe asynchronous_communication.cpp
 * Run: mpirun -np 2 ./async_probe
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
        // Sender: Create a message of varying size
        std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::cout << "Process 0: Sending " << data.size() << " integers"
                  << std::endl;

        MPI_Send(data.data(), static_cast<int>(data.size()), MPI_INT, 1, tag,
                 MPI_COMM_WORLD);

    } else if (rank == 1) {
        MPI_Status status;

        // First, probe to find out the message size
        std::cout << "Process 1: Probing for message..." << std::endl;
        MPI_Probe(0, tag, MPI_COMM_WORLD, &status);

        // Get the number of elements in the message
        int count = 0;
        MPI_Get_count(&status, MPI_INT, &count);
        std::cout << "Process 1: Message has " << count << " elements"
                  << std::endl;

        // Allocate buffer of exactly the right size
        std::vector<int> data(count);

        // Now receive the message
        MPI_Recv(data.data(), count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Print received data
        std::cout << "Process 1: Received data: ";
        for (int val : data) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
