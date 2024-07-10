#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank (ID) of the current process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define the root process
    int root = 0;

    // Create an array to be scattered by the root process
    std::vector<int> data;
    if (rank == root) {
        data.resize(size * 2);
        for (int i = 0; i < size * 2; ++i) {
            data[i] = i;
        }
        std::cout << "Process " << rank << " has data: ";
        for (const auto& val : data) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    // Prepare a buffer for receiving the scattered data
    std::vector<int> recvbuf(2);

    // Scatter the data from the root process to all processes
    MPI_Scatter(data.data(), 2, MPI_INT, recvbuf.data(), 2, MPI_INT, root, MPI_COMM_WORLD);
    std::cout << "Process " << rank << " received data: ";
    for (const auto& val : recvbuf) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Perform some operation on the received data
    // (For example, multiply each element by 2)
    for (auto& val : recvbuf) {
        val *= 2;
    }
    std::cout << "Process " << rank << " processed data: ";
    for (const auto& val : recvbuf) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Gather the processed data back to the root process
    std::vector<int> gathered_data;
    if (rank == root) {
        gathered_data.resize(size * 2);
    }

    MPI_Gather(recvbuf.data(), 2, MPI_INT, gathered_data.data(), 2, MPI_INT, root, MPI_COMM_WORLD);

    // The root process prints the gathered data
    if (rank == root) {
        std::cout << "Process " << rank << " gathered data: ";
        for (const auto& val : gathered_data) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
