#include <mpi.h>
#include <iostream>
#include <vector>

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
    std::cout << "Hello world from process " << rank << " out of " << size << " processes" << std::endl;

    // Simple demonstration of sending and receiving messages
    if (rank == 0) {
        // If we are the master process (rank 0), send a message to all other processes
        for (int i = 1; i < size; ++i) {
            std::string message = "Hello from master process to process " + std::to_string(i);
            MPI_Send(message.c_str(), message.size() + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            std::cout << "Master process sent message to process " << i << std::endl;
        }
    } else {
        // If we are a worker process, receive the message from the master process
        char message[100];
        MPI_Recv(message, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " received message: '" << message << "'" << std::endl;
    }

    // Barrier to synchronize processes
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Process " << rank << " has reached the barrier" << std::endl;

    // More complex example: gathering data from all processes
    int data = rank * 2;  // Each process has some data
    std::cout << "Process " << rank << " has data: " << data << std::endl;

    // Collect all data at the master process
    std::vector<int> gathered_data(size);
    MPI_Gather(&data, 1, MPI_INT, gathered_data.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Master process has gathered data: ";
        for (int i = 0; i < size; ++i) {
            std::cout << gathered_data[i] << " ";
        }
        std::cout << std::endl;
    }

    // Broadcasting data from master to all processes
    char broadcast_data[50];
    if (rank == 0) {
        std::strcpy(broadcast_data, "Broadcasting from master");
    }
    MPI_Bcast(broadcast_data, 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    std::cout << "Process " << rank << " received broadcast data: " << broadcast_data << std::endl;

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
