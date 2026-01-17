/**
 * Sendrecv Operation
 *
 * This example demonstrates MPI_Sendrecv, which combines a send and receive
 * into a single call. This is particularly useful for avoiding deadlocks
 * in exchange patterns where processes need to both send and receive.
 *
 * Key concepts:
 * - MPI_Sendrecv: Atomic send and receive operation
 * - Deadlock avoidance: No risk of circular wait
 * - Can send to one process and receive from a different one
 * - Useful for neighbor exchange patterns
 *
 * Problem with separate Send/Recv:
 *   Process 0: Send to 1, then Recv from 1  // DEADLOCK!
 *   Process 1: Send to 0, then Recv from 0  // Both waiting to send
 *
 * Solution with Sendrecv:
 *   Both processes: Sendrecv(to_neighbor, from_neighbor)  // Safe!
 *
 * Compile: mpicxx -o sendrecv sendrecv_operation.cpp
 * Run: mpirun -np 4 ./sendrecv
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

    // Example 1: Simple pairwise exchange
    // Even ranks exchange with rank+1, odd ranks exchange with rank-1
    if (rank < size - (size % 2)) {  // Ensure we have pairs
        int partner = (rank % 2 == 0) ? rank + 1 : rank - 1;

        int send_data = rank * 100;
        int recv_data = 0;
        constexpr int tag = 0;

        std::cout << "Process " << rank << ": Exchanging with process "
                  << partner << ", sending " << send_data << std::endl;

        MPI_Sendrecv(&send_data, 1, MPI_INT, partner, tag,
                     &recv_data, 1, MPI_INT, partner, tag,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::cout << "Process " << rank << ": Received " << recv_data
                  << " from process " << partner << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Example 2: Ring shift (each process sends to next, receives from previous)
    if (rank == 0) {
        std::cout << std::endl << "=== Ring Shift ===" << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    int left_neighbor = (rank - 1 + size) % size;   // Previous in ring
    int right_neighbor = (rank + 1) % size;         // Next in ring

    int my_value = rank;
    int received_value = 0;
    constexpr int ring_tag = 1;

    // Send my value to the right, receive from the left
    MPI_Sendrecv(&my_value, 1, MPI_INT, right_neighbor, ring_tag,
                 &received_value, 1, MPI_INT, left_neighbor, ring_tag,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    std::cout << "Process " << rank << ": Sent " << my_value << " to "
              << right_neighbor << ", received " << received_value << " from "
              << left_neighbor << std::endl;

    // Example 3: Sendrecv_replace - uses same buffer for send and receive
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << std::endl << "=== Sendrecv_replace ===" << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    int value = rank * 10;
    std::cout << "Process " << rank << ": Before replace, value = " << value
              << std::endl;

    // Send to right, receive from left, using same buffer
    MPI_Sendrecv_replace(&value, 1, MPI_INT, right_neighbor, ring_tag,
                         left_neighbor, ring_tag, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

    std::cout << "Process " << rank << ": After replace, value = " << value
              << std::endl;

    MPI_Finalize();
    return 0;
}
