from mpi4py import MPI

# Initialize the MPI communication world
comm = MPI.COMM_WORLD

# Get the total number of processes
size = comm.Get_size()

# Get the rank (ID) of the current process
rank = comm.Get_rank()

# Print a greeting from each process
print(f"Hello world from process {rank} out of {size} processes")

# Simple demonstration of sending and receiving messages
if rank == 0:
    # If we are the master process (rank 0), send a message to all other processes
    for i in range(1, size):
        message = f"Hello from master process to process {i}"
        comm.send(message, dest=i, tag=11)
        print(f"Master process sent message to process {i}")
else:
    # If we are a worker process, receive the message from the master process
    message = comm.recv(source=0, tag=11)
    print(f"Process {rank} received message: '{message}'")

# Barrier to synchronize processes
comm.Barrier()
print(f"Process {rank} has reached the barrier")

# More complex example: gathering data from all processes
data = rank * 2  # Each process has some data
print(f"Process {rank} has data: {data}")

# Collect all data at the master process
gathered_data = comm.gather(data, root=0)

if rank == 0:
    print(f"Master process has gathered data: {gathered_data}")

# Broadcasting data from master to all processes
if rank == 0:
    broadcast_data = "Broadcasting from master"
else:
    broadcast_data = None

broadcast_data = comm.bcast(broadcast_data, root=0)
print(f"Process {rank} received broadcast data: {broadcast_data}")

# Finalize the MPI environment (optional, done automatically at the end of the script)
MPI.Finalize()
