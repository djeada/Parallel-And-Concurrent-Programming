// Program 5: Non-Blocking Communication
#include <mpi.h>
#include <iostream>
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Request request;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        int data = 100;
        MPI_Isend(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    } else if (rank == 1) {
        int recv_data;
        MPI_Irecv(&recv_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    }
    MPI_Finalize();
    return 0;
}
