// Program 8: Cartesian Topology
#include <mpi.h>
#include <iostream>
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int dims[2] = {0, 0}, periods[2] = {0, 0}, coords[2];
    MPI_Comm cart_comm;
    MPI_Dims_create(4, 2, dims);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);
    int rank;
    MPI_Comm_rank(cart_comm, &rank);
    MPI_Cart_coords(cart_comm, rank, 2, coords);
    MPI_Finalize();
    return 0;
}
