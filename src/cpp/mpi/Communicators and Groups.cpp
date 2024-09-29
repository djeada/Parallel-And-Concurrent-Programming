// Program 7: Communicators and Groups
#include <mpi.h>
#include <iostream>
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, new_rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm new_comm;
    int color = rank % 2;
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &new_comm);
    MPI_Comm_rank(new_comm, &new_rank);
    MPI_Finalize();
    return 0;
}
