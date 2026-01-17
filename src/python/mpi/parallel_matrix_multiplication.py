"""
Parallel Matrix Multiplication

This example demonstrates parallel matrix multiplication using
MPI Scatter and Gather. Each process computes a portion of
the result matrix.

Key concepts:
- comm.Scatter: Distributes rows of matrix A to all processes
- comm.Bcast: Broadcasts the entire B matrix to all processes
- comm.Gather: Collects results back to root
- Data decomposition: Each process handles rows_per_process rows

Algorithm:
1. Root initializes matrices A and B
2. Scatter rows of A to all processes
3. Broadcast entire B to all processes
4. Each process computes its portion of C = A * B
5. Gather results back to root

Note: This assumes matrix size N is evenly divisible by process count.

Run: mpirun -np 4 python parallel_matrix_multiplication.py
"""

from mpi4py import MPI
import numpy as np


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    N = 4  # Matrix size (N x N)

    # Check that N is divisible by process count
    if N % size != 0:
        if rank == 0:
            print(f"Error: Matrix size {N} must be divisible by process count {size}")
        return

    rows_per_process = N // size

    # Root initializes matrices
    if rank == 0:
        print(f"Parallel Matrix Multiplication ({N}x{N}) with {size} processes")
        print(f"Each process handles {rows_per_process} rows")
        print()

        # Initialize A and B with simple values
        A = np.arange(N * N, dtype='d').reshape(N, N)
        B = np.arange(N * N, dtype='d').reshape(N, N) % N

        print("Matrix A:")
        print(A)
        print()
        print("Matrix B:")
        print(B)
        print()
    else:
        A = None
        B = np.empty((N, N), dtype='d')

    # Local portion of A
    local_A = np.empty((rows_per_process, N), dtype='d')

    # Scatter rows of A to all processes
    comm.Scatter(A, local_A, root=0)

    # Broadcast entire B to all processes
    comm.Bcast(B, root=0)

    # Each process computes its portion of C
    local_C = np.dot(local_A, B)

    print(f"Process {rank}: Computed {rows_per_process} rows of result")

    # Gather results back to root
    if rank == 0:
        C = np.empty((N, N), dtype='d')
    else:
        C = None

    comm.Gather(local_C, C, root=0)

    # Root prints the result
    if rank == 0:
        print()
        print("Result Matrix C = A * B:")
        print(C)


if __name__ == "__main__":
    main()
