/**
 * Hello World MPI - Basic Initialization and Finalization
 *
 * This example demonstrates the fundamental structure of an MPI program.
 * Every MPI program must call MPI_Init() before any other MPI function
 * and MPI_Finalize() before exiting.
 *
 * Key concepts:
 * - MPI_Init: Initializes the MPI execution environment
 * - MPI_Finalize: Terminates the MPI execution environment
 * - MPI_Comm_rank: Gets the rank (unique ID) of the calling process
 * - MPI_Comm_size: Gets the total number of processes
 * - MPI_COMM_WORLD: Default communicator containing all processes
 *
 * Compile: mpicxx -o hello_world_mpi hello_world_mpi.cpp
 * Run: mpirun -np 4 ./hello_world_mpi
 *
 * Expected output (order may vary):
 *   Hello from rank 0 out of 4 processes
 *   Hello from rank 1 out of 4 processes
 *   Hello from rank 2 out of 4 processes
 *   Hello from rank 3 out of 4 processes
 */

#include <mpi.h>

#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize MPI environment - must be called first
    MPI_Init(&argc, &argv);

    // Get the rank (unique identifier) of this process
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes in the communicator
    int size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Each process prints its greeting
    std::cout << "Hello from rank " << rank << " out of " << size
              << " processes" << std::endl;

    // Finalize MPI environment - must be called before exiting
    MPI_Finalize();

    return 0;
}
