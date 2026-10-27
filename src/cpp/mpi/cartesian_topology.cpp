/**
 * C++20 + MPI: scoped environment and checked calls through the supported C API.
 *
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
 * Compile: mpicxx -std=c++20 -o cart_topology cartesian_topology.cpp
 * Run: mpirun -np 6 ./cart_topology
 */

#include "../common/mpi_support.hpp"

#include <array>
#include <iostream>

int main(int argc, char* argv[]) {
    return mpi_example::run(argc, argv, [&]() -> int {

        int world_rank = 0;
        int world_size = 0;
        mpi_example::check(MPI_Comm_rank(MPI_COMM_WORLD, &world_rank));
        mpi_example::check(MPI_Comm_size(MPI_COMM_WORLD, &world_size));

        // Create a 2D Cartesian grid
        constexpr int ndims = 2;

        // Let MPI determine optimal dimensions
        std::array<int, ndims> dims{0, 0};
        mpi_example::check(MPI_Dims_create(world_size, ndims, dims.data()));

        if (world_rank == 0) {
            std::cout << "Creating " << dims[0] << "x" << dims[1]
                      << " Cartesian grid with " << world_size << " processes"
                      << std::endl;
        }

        // Periodic boundaries: 1 = periodic (wraps around), 0 = non-periodic
        std::array<int, ndims> periods{1, 1};  // Periodic in both dimensions (torus)

        // reorder = 1 allows MPI to reorder ranks for efficiency
        int reorder = 1;

        // Create the Cartesian communicator
        mpi_example::Communicator cart_comm;
        mpi_example::check(MPI_Cart_create(MPI_COMM_WORLD, ndims, dims.data(), periods.data(), reorder, cart_comm.out()));

        // Get rank and coordinates in the new communicator
        int cart_rank = 0;
        mpi_example::check(MPI_Comm_rank(cart_comm.get(), &cart_rank));

        std::array<int, ndims> coords{0, 0};
        mpi_example::check(MPI_Cart_coords(cart_comm.get(), cart_rank, ndims, coords.data()));

        std::cout << "World rank " << world_rank << " -> Cart rank " << cart_rank
                  << " at coords (" << coords[0] << ", " << coords[1] << ")"
                  << std::endl;

        // Find neighbors using MPI_Cart_shift
        // Direction 0 = row (up/down), Direction 1 = column (left/right)
        int up = 0;
        int down = 0;
        int left = 0;
        int right = 0;

        mpi_example::check(MPI_Cart_shift(cart_comm.get(), 0, 1, &up, &down));      // Shift in row direction
        mpi_example::check(MPI_Cart_shift(cart_comm.get(), 1, 1, &left, &right));  // Shift in column direction

        std::cout << "Cart rank " << cart_rank << " neighbors: up=" << up
                  << ", down=" << down << ", left=" << left << ", right=" << right
                  << std::endl;

        // cart_comm is freed before MPI finalizes.
        return 0;
    });
}
