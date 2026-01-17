/**
 * Cartesian Topology
 *
 * This example demonstrates how to create a Cartesian topology using
 * MPI_Cart_create. Cartesian topologies are useful for problems with
 * regular grid structures, like finite difference methods.
 *
 * Key concepts:
 * - MPI_Cart_create: Creates a Cartesian communicator
 * - MPI_Dims_create: Helper to determine optimal grid dimensions
 * - MPI_Cart_coords: Gets coordinates of a process in the grid
 * - MPI_Cart_rank: Gets rank from coordinates
 * - MPI_Cart_shift: Finds neighbor ranks in a given direction
 * - Periodic boundaries: Can wrap around edges (like a torus)
 *
 * Use cases:
 * - Finite difference/element methods
 * - Image processing
 * - Any algorithm with regular grid data decomposition
 *
 * Compile: mpicxx -o cart_topology cartesian_topology.cpp
 * Run: mpirun -np 6 ./cart_topology
 */

#include <mpi.h>

#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank = 0;
    int world_size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Create a 2D Cartesian grid
    constexpr int ndims = 2;

    // Let MPI determine optimal dimensions
    int dims[ndims] = {0, 0};
    MPI_Dims_create(world_size, ndims, dims);

    if (world_rank == 0) {
        std::cout << "Creating " << dims[0] << "x" << dims[1]
                  << " Cartesian grid with " << world_size << " processes"
                  << std::endl;
    }

    // Periodic boundaries: 1 = periodic (wraps around), 0 = non-periodic
    int periods[ndims] = {1, 1};  // Periodic in both dimensions (torus)

    // reorder = 1 allows MPI to reorder ranks for efficiency
    int reorder = 1;

    // Create the Cartesian communicator
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &cart_comm);

    // Get rank and coordinates in the new communicator
    int cart_rank = 0;
    MPI_Comm_rank(cart_comm, &cart_rank);

    int coords[ndims] = {0, 0};
    MPI_Cart_coords(cart_comm, cart_rank, ndims, coords);

    std::cout << "World rank " << world_rank << " -> Cart rank " << cart_rank
              << " at coords (" << coords[0] << ", " << coords[1] << ")"
              << std::endl;

    // Find neighbors using MPI_Cart_shift
    // Direction 0 = row (up/down), Direction 1 = column (left/right)
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;

    MPI_Cart_shift(cart_comm, 0, 1, &up, &down);      // Shift in row direction
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);  // Shift in column direction

    std::cout << "Cart rank " << cart_rank << " neighbors: up=" << up
              << ", down=" << down << ", left=" << left << ", right=" << right
              << std::endl;

    // Clean up
    MPI_Comm_free(&cart_comm);

    MPI_Finalize();
    return 0;
}
