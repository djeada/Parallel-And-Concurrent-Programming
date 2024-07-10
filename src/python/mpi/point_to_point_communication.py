from mpi4py import MPI

# Initialize the MPI communication world
comm = MPI.COMM_WORLD

# Get the rank (ID) of the current process
rank = comm.rank

# Print a greeting from each process
print(f"Hello from process {rank}")

# Process with rank 0 sends an integer to process with rank 4
if rank == 0:
    data = 10000000  # Data to be sent
    destination_process = 4  # Destination process rank
    comm.send(data, dest=destination_process)
    print(f"Process {rank} sent {data} to process {destination_process}")

# Process with rank 1 sends a string to process with rank 8
if rank == 1:
    data = "hello"  # Data to be sent
    destination_process = 8  # Destination process rank
    comm.send(data, dest=destination_process)
    print(f"Process {rank} sent {data} to process {destination_process}")

# Process with rank 4 receives data from process with rank 0
if rank == 4:
    source_process = 0  # Source process rank
    data = comm.recv(source=source_process)
    print(f"Process {rank} received {data} from process {source_process}")

# Process with rank 8 receives data from process with rank 1
if rank == 8:
    source_process = 1  # Source process rank
    data = comm.recv(source=source_process)
    print(f"Process {rank} received {data} from process {source_process}")

# Barrier to synchronize processes
comm.Barrier()
print(f"Process {rank} has reached the barrier")

# Process with rank 0 broadcasts a message to all other processes
if rank == 0:
    broadcast_data = "Broadcasting from master"
else:
    broadcast_data = None  # Placeholder for other processes

# Broadcasting the message from process 0 to all processes
broadcast_data = comm.bcast(broadcast_data, root=0)
print(f"Process {rank} received broadcast data: {broadcast_data}")

# More complex example: gathering data from all processes at process with rank 0
data = rank * 2  # Each process generates its own data

# Collect all data at the master process (rank 0)
gathered_data = comm.gather(data, root=0)

if rank == 0:
    print(f"Master process gathered data: {gathered_data}")

# Finalize the MPI environment (optional, done automatically at the end of the script)
MPI.Finalize()
