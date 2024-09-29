// Program 6: Derived Data Types
#include <mpi.h>
#include <iostream>
struct Particle {
    double x, y, z;
    double vx, vy, vz;
};
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Datatype particle_type;
    int block_lengths[2] = {3, 3};
    MPI_Aint offsets[2];
    offsets[0] = offsetof(Particle, x);
    offsets[1] = offsetof(Particle, vx);
    MPI_Datatype types[2] = {MPI_DOUBLE, MPI_DOUBLE};
    MPI_Type_create_struct(2, block_lengths, offsets, types, &particle_type);
    MPI_Type_commit(&particle_type);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        Particle p = {0.0, 1.0, 2.0, 0.5, 0.5, 0.5};
        MPI_Send(&p, 1, particle_type, 1, 0, MPI_COMM_WORLD);
    } else if (rank == 1) {
        Particle p;
        MPI_Recv(&p, 1, particle_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Type_free(&particle_type);
    MPI_Finalize();
    return 0;
}
