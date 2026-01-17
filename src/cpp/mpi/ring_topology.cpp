/**
 * Ring Topology Communication
 *
 * This example demonstrates a ring communication pattern where each
 * process passes data to its neighbor in a circular fashion. This is
 * a fundamental pattern in many parallel algorithms.
 *
 * Key concepts:
 * - Ring topology: Processes arranged in a circle (0->1->2->...->0)
 * - Token passing: Data circulates through all processes
 * - Accumulation: Each process can contribute to the circulating data
 *
 * Applications:
 * - Token ring algorithms
 * - Pipeline computations
 * - Circular data redistribution
 * - Some collective operation implementations
 *
 * Pattern:
 *   Process 0 -> Process 1 -> Process 2 -> ... -> Process 0
 *
 * Compile: mpicxx -o ring_topology ring_topology.cpp
 * Run: mpirun -np 4 ./ring_topology
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

    // Calculate neighbors in the ring
    int left = (rank - 1 + size) % size;
    int right = (rank + 1) % size;

    std::cout << "Process " << rank << ": left neighbor = " << left
              << ", right neighbor = " << right << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);

    // Example 1: Simple token passing around the ring
    if (rank == 0) {
        std::cout << std::endl << "=== Token Passing ===" << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    constexpr int tag = 0;
    int token = 0;

    if (rank == 0) {
        // Process 0 starts the token
        token = 42;
        std::cout << "Process 0: Starting token with value " << token
                  << std::endl;
        MPI_Send(&token, 1, MPI_INT, right, tag, MPI_COMM_WORLD);
        MPI_Recv(&token, 1, MPI_INT, left, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        std::cout << "Process 0: Token returned with value " << token
                  << std::endl;
    } else {
        // Other processes receive, modify, and pass the token
        MPI_Recv(&token, 1, MPI_INT, left, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << ": Received token with value "
                  << token << std::endl;
        token += rank;  // Each process adds its rank
        std::cout << "Process " << rank << ": Passing token with value "
                  << token << std::endl;
        MPI_Send(&token, 1, MPI_INT, right, tag, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Example 2: Ring allgather - each process shares data with all others
    if (rank == 0) {
        std::cout << std::endl << "=== Ring Allgather ===" << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // Each process has some local data
    int my_data = rank * 100;
    std::vector<int> all_data(size);
    all_data[rank] = my_data;

    std::cout << "Process " << rank << ": my_data = " << my_data << std::endl;

    // Pass data around the ring, accumulating all values
    int send_data = my_data;
    for (int step = 0; step < size - 1; ++step) {
        int recv_data = 0;

        // Use Sendrecv to avoid deadlock
        MPI_Sendrecv(&send_data, 1, MPI_INT, right, tag,
                     &recv_data, 1, MPI_INT, left, tag,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Store received data in the correct position
        // Data from rank r arrives at step (rank - r + size) % size - 1
        int source_rank = (rank - step - 1 + size) % size;
        all_data[source_rank] = recv_data;

        // Pass along what we received
        send_data = recv_data;
    }

    // Each process now has all data
    std::cout << "Process " << rank << ": all_data = [";
    for (int i = 0; i < size; ++i) {
        std::cout << all_data[i];
        if (i < size - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    MPI_Finalize();
    return 0;
}
