/**
 * Derived Data Types
 *
 * This example demonstrates MPI derived data types for sending complex
 * data structures. Instead of serializing data manually, MPI allows you
 * to define custom types that describe the memory layout of your structures.
 *
 * Key concepts:
 * - MPI_Type_create_struct: Creates a type for a C/C++ struct
 * - MPI_Type_commit: Commits the type for use in communication
 * - MPI_Type_free: Frees the type when done
 * - offsetof: Gets byte offset of struct members
 *
 * Other derived type constructors:
 * - MPI_Type_contiguous: Contiguous copies of a base type
 * - MPI_Type_vector: Regularly spaced blocks
 * - MPI_Type_indexed: Irregularly spaced blocks
 *
 * Benefits:
 * - Send complex data structures in a single call
 * - No need for manual packing/unpacking
 * - MPI can optimize the data transfer
 *
 * Compile: mpicxx -o derived_types derived_data_types.cpp
 * Run: mpirun -np 2 ./derived_types
 */

#include <mpi.h>

#include <cstddef>
#include <iostream>

// Define a structure representing a particle in a simulation
struct Particle {
    double position[3];  // x, y, z coordinates
    double velocity[3];  // vx, vy, vz velocity components
    int id;              // particle identifier
    double mass;         // particle mass
};

// Creates an MPI derived type for the Particle struct
MPI_Datatype create_particle_type() {
    // Number of blocks (groups of contiguous elements)
    constexpr int num_blocks = 4;

    // Number of elements in each block
    int block_lengths[num_blocks] = {3, 3, 1, 1};

    // Byte offsets of each block from the start of the struct
    MPI_Aint offsets[num_blocks];
    offsets[0] = offsetof(Particle, position);
    offsets[1] = offsetof(Particle, velocity);
    offsets[2] = offsetof(Particle, id);
    offsets[3] = offsetof(Particle, mass);

    // MPI types for each block
    MPI_Datatype types[num_blocks] = {MPI_DOUBLE, MPI_DOUBLE, MPI_INT,
                                      MPI_DOUBLE};

    // Create the derived type
    MPI_Datatype particle_type;
    MPI_Type_create_struct(num_blocks, block_lengths, offsets, types,
                           &particle_type);

    // Commit the type (required before use)
    MPI_Type_commit(&particle_type);

    return particle_type;
}

void print_particle(const char* prefix, const Particle& p) {
    std::cout << prefix << " Particle " << p.id << ":"
              << " pos=(" << p.position[0] << ", " << p.position[1] << ", "
              << p.position[2] << ")"
              << " vel=(" << p.velocity[0] << ", " << p.velocity[1] << ", "
              << p.velocity[2] << ")"
              << " mass=" << p.mass << std::endl;
}

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

    // Create the derived type
    MPI_Datatype particle_type = create_particle_type();
    constexpr int tag = 0;

    if (rank == 0) {
        // Initialize a particle
        Particle p = {{1.0, 2.0, 3.0}, {0.1, 0.2, 0.3}, 42, 1.5};

        std::cout << "Process 0: Sending particle" << std::endl;
        print_particle("Process 0:", p);

        // Send the particle using the derived type
        MPI_Send(&p, 1, particle_type, 1, tag, MPI_COMM_WORLD);

    } else if (rank == 1) {
        // Receive the particle
        Particle p = {};
        MPI_Recv(&p, 1, particle_type, 0, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        std::cout << "Process 1: Received particle" << std::endl;
        print_particle("Process 1:", p);
    }

    // Clean up the derived type
    MPI_Type_free(&particle_type);

    MPI_Finalize();
    return 0;
}
