// Program 4: Reduce Operation
#include <mpi.h>
#include <iostream>
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, sum;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int value = rank + 1;
    MPI_Reduce(&value, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
