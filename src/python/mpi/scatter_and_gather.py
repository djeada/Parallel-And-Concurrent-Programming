
from mpi4py import MPI
import numpy as np

# Initialize the MPI environment
comm = MPI.COMM_WORLD

# Get the rank (ID) of the current process
rank = comm.Get_rank()

# Get the total number of processes
size = comm.Get_size()

# Define the root process
root = 0

# Create an array to be scattered by the root process
if rank == root:
    data = np.arange(size * 2, dtype='i')  # Example array: [0, 1, 2, ..., size*2-1]
    print(f"Process {rank} has data: {data}")
else:
    data = None

# Prepare a buffer for receiving the scattered data
recvbuf = np.empty(2, dtype='i')

# Scatter the data from the root process to all processes
comm.Scatter(data, recvbuf, root=root)
print(f"Process {rank} received data: {recvbuf}")

# Perform some operation on the received data
# (For example, multiply each element by 2)
recvbuf = recvbuf * 2
print(f"Process {rank} processed data: {recvbuf}")

# Gather the processed data back to the root process
gathered_data = None
if rank == root:
    gathered_data = np.empty(size * 2, dtype='i')

comm.Gather(recvbuf, gathered_data, root=root)

# The root process prints the gathered data
if rank == root:
    print(f"Process {rank} gathered data: {gathered_data}")

# Finalize the MPI environment (optional, done automatically at the end of the script)
MPI.Finalize()
