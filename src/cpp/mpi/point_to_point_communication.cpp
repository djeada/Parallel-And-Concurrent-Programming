#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstring>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the total number of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the rank (ID) of the current process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Print a greeting from each process
    std::cout << "Hello from process " << rank << std::endl;

    // Process with rank 0 sends an integer to process with rank 4
    if (rank == 0) {
        int data = 10000000;  // Data to be sent
        int destination_process = 4;  // Destination process rank
        MPI_Send(&data, 1, MPI_INT, destination_process, 0, MPI_COMM_WORLD);
        std::cout << "Process " << rank << " sent " << data << " to process " << destination_process << std::endl;
    }

    // Process with rank 1 sends a string to process with rank 8
    if (rank == 1) {
        const char* data = "hello";  // Data to be sent
        int destination_process = 8;  // Destination process rank
        MPI_Send(data, strlen(data) + 1, MPI_CHAR, destination_process, 0, MPI_COMM_WORLD);
        std::cout << "Process " << rank << " sent " << data << " to process " << destination_process << std::endl;
    }

    // Process with rank 4 receives data from process with rank 0
    if (rank == 4) {
        int data;
        int source_process = 0;  // Source process rank
        MPI_Recv(&data, 1, MPI_INT, source_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " received " << data << " from process " << source_process << std::endl;
    }

    // Process with rank 8 receives data from process with rank 1
    if (rank == 8) {
        char data[100];
        int source_process = 1;  // Source process rank
        MPI_Recv(data, 100, MPI_CHAR, source_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " received " << data << " from process " << source_process << std::endl;
    }

    // Barrier to synchronize processes
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Process " << rank << " has reached the barrier" << std::endl;

    // Process with rank 0 broadcasts a message to all other processes
    char broadcast_data[50];
    if (rank == 0) {
        std::strcpy(broadcast_data, "Broadcasting from master");
    }
    MPI_Bcast(broadcast_data, 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    std::cout << "Process " << rank << " received broadcast data: " << broadcast_data << std::endl;

    // More complex example: gathering data from all processes at process with rank 0
    int data = rank * 2;  // Each process generates its own data

    // Collect all data at the master process (rank 0)
    std::vector<int> gathered_data(size);
    MPI_Gather(&data, 1, MPI_INT, gathered_data.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Master process gathered data: ";
        for (int i = 0; i < size; ++i) {
            std::cout << gathered_data[i] << " ";
        }
        std::cout << std::endl;
    }

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}
