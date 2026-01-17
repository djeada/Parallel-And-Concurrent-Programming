/**
 * Barrier Synchronization
 *
 * This example demonstrates MPI_Barrier, which synchronizes all processes
 * in a communicator. No process can proceed past the barrier until all
 * processes have reached it.
 *
 * Key concepts:
 * - MPI_Barrier: Blocks until all processes in the communicator call it
 * - Synchronization point: Ensures all processes are at the same stage
 * - Use sparingly: Barriers can hurt performance if overused
 *
 * Common use cases:
 * - Ensuring all processes have completed a phase before starting the next
 * - Synchronizing before timing measurements
 * - Ensuring file I/O is complete before reading results
 *
 * Note: Many collective operations (Bcast, Reduce, etc.) have implicit
 * synchronization, so explicit barriers are often unnecessary.
 *
 * Compile: mpicxx -o barrier barrier_synchronization.cpp
 * Run: mpirun -np 4 ./barrier
 */

#include <mpi.h>

#include <chrono>
#include <iostream>
#include <thread>

// Simulate variable work with different durations
void do_work(int rank) {
    // Each process works for a different amount of time
    int sleep_ms = (rank + 1) * 200;  // 200ms, 400ms, 600ms, ...
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Phase 1: Each process does variable-length work
    std::cout << "Process " << rank << ": Starting Phase 1 work..." << std::endl;
    do_work(rank);
    std::cout << "Process " << rank << ": Phase 1 complete" << std::endl;

    // Barrier: Wait for all processes to complete Phase 1
    std::cout << "Process " << rank << ": Waiting at barrier..." << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Process " << rank << ": Passed barrier" << std::endl;

    // Phase 2: All processes start together
    if (rank == 0) {
        std::cout << std::endl;
        std::cout << "=== All processes synchronized ===" << std::endl;
        std::cout << "=== Starting Phase 2 ===" << std::endl;
        std::cout << std::endl;
    }

    // Another barrier to ensure the message is printed before Phase 2 output
    MPI_Barrier(MPI_COMM_WORLD);

    std::cout << "Process " << rank << ": Phase 2 starting now" << std::endl;

    // Demonstrate timing with barrier synchronization
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // All processes do the same work
    do_work(0);  // Same duration for all

    double end_time = MPI_Wtime();
    double local_elapsed = end_time - start_time;

    // Report timing
    std::cout << "Process " << rank << ": Elapsed time = " << local_elapsed
              << " seconds" << std::endl;

    // Final barrier before exit
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << std::endl;
        std::cout << "All processes completed successfully" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
