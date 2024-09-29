
#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size, n, i;
    double h, local_sum, x, pi, total_sum;

    MPI_Init(&argc, &argv);               // Initialize MPI environment

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank

    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get size

    n = 1000000; // Number of intervals
    h = 1.0 / (double)n;
    local_sum = 0.0;

    // Compute partial sum
    for (i = rank; i < n; i += size) {
        x = h * ((double)i + 0.5);
        local_sum += 4.0 / (1.0 + x * x);

    }

    local_sum *= h;

    // Reduce all partial sums to the total sum

    MPI_Reduce(&local_sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Approximate value of Pi: %.16f\n", pi);

    }

    MPI_Finalize();
    return 0;

}
