/**
 * Communicators and Groups
 *
 * This example demonstrates how to create sub-communicators using
 * MPI_Comm_split. This is useful for organizing processes into groups
 * for modular parallel algorithms.
 *
 * Key concepts:
 * - MPI_Comm_split: Splits a communicator into sub-communicators
 * - Color: Determines which sub-communicator a process joins
 * - Key: Determines the rank ordering within the new communicator
 * - Processes with the same color join the same sub-communicator
 * - MPI_UNDEFINED color excludes a process from any sub-communicator
 *
 * Use cases:
 * - Hierarchical parallelism (e.g., nodes vs cores)
 * - Isolating communication in different parts of the program
 * - Creating library-safe communication contexts
 *
 * Compile: mpicxx -o comm_split communicators_and_groups.cpp
 * Run: mpirun -np 8 ./comm_split
 */

#include <mpi.h>

#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank = 0;
    int world_size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Split processes into two groups based on rank parity
    // Even ranks (0, 2, 4, ...) get color 0
    // Odd ranks (1, 3, 5, ...) get color 1
    int color = world_rank % 2;

    // Use world_rank as key to preserve relative ordering
    MPI_Comm new_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &new_comm);

    // Get rank within the new communicator
    int new_rank = 0;
    int new_size = 0;
    MPI_Comm_rank(new_comm, &new_rank);
    MPI_Comm_size(new_comm, &new_size);

    std::cout << "World rank " << world_rank << " -> Group "
              << (color == 0 ? "EVEN" : "ODD") << ", new rank " << new_rank
              << " of " << new_size << std::endl;

    // Demonstrate communication within the sub-communicator
    // Reduce within each group separately
    int group_sum = 0;
    MPI_Reduce(&world_rank, &group_sum, 1, MPI_INT, MPI_SUM, 0, new_comm);

    if (new_rank == 0) {
        std::cout << "Group " << (color == 0 ? "EVEN" : "ODD")
                  << " sum of world ranks: " << group_sum << std::endl;
    }

    // Clean up the new communicator
    MPI_Comm_free(&new_comm);

    MPI_Finalize();
    return 0;
}
