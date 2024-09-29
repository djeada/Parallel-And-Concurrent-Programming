// Program 9: Asynchronous Communication with MPI_Probe
#include <mpi.h>
#include <iostream>
#include <vector>
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, flag;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::vector<int> data(100, 1);
        MPI_Send(data.data(), data.size(), MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else if (rank == 1) {
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_INT, &count);
        std::vector<int> data(count);
        MPI_Recv(data.data(), count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Finalize();
    return 0;
}
