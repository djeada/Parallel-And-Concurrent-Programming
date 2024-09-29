// Program 10: Parallel Matrix Multiplication
#include <mpi.h>
#include <iostream>
#include <vector>
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size, N = 4;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rows_per_proc = N / size;
    std::vector<double> A(N * N), B(N * N), C(N * N, 0.0);
    if (rank == 0) {
        for (int i = 0; i < N * N; i++) {
            A[i] = i;
            B[i] = i;
        }
    }
    std::vector<double> local_A(rows_per_proc * N);
    MPI_Scatter(A.data(), rows_per_proc * N, MPI_DOUBLE, local_A.data(), rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B.data(), N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    std::vector<double> local_C(rows_per_proc * N, 0.0);
    for (int i = 0; i < rows_per_proc; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                local_C[i * N + j] += local_A[i * N + k] * B[k * N + j];
    MPI_Gather(local_C.data(), rows_per_proc * N, MPI_DOUBLE, C.data(), rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
