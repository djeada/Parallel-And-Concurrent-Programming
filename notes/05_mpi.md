## Message Passing Interface (MPI)

MPI is a standardized and portable message-passing system designed to enable parallelism in distributed-memory systems, where each process has its own memory. It facilitates communication between processes through message passing, allowing programs to run concurrently on multiple processors or computers connected via a network.

### Key Concepts

#### MPI Environment

- MPI_Init: Initializes the MPI environment and must be called before any other MPI functions.
- MPI_Finalize: Cleans up the MPI environment and must be called before a program exits.

#### Communicators and Ranks

- Communicator: A group of MPI processes that can communicate with each other.
- Rank: A unique identifier assigned to each process within a communicator.

#### Point-to-Point Communication

- MPI_Send: Sends a message from one process to another.
- MPI_Recv: Receives a message from another process.
- Blocking and non-blocking communication: Blocking communication waits for the message to be sent or received before continuing, while non-blocking communication allows the program to continue executing while the message is being sent or received.

#### Collective Communication

- MPI_Barrier: Synchronizes all processes within a communicator, ensuring that all processes reach the barrier before continuing.
- MPI_Bcast: Broadcasts a message from one process (the root) to all other processes in a communicator.
- MPI_Scatter: Distributes an array of data from the root process to all other processes in a communicator.
- MPI_Gather: Gathers data from all processes in a communicator and stores it in an array on the root process.
- MPI_Reduce: Performs a reduction operation (e.g., sum, product) on data from all processes in a communicator and stores the result on the root process.

### Best Practices

When using MPI, consider the following best practices:

1. Minimize communication overhead: Limit the number and size of messages to reduce the time spent on communication.
2. Use non-blocking communication when possible: This allows the program to continue executing while messages are being sent or received.
3. Use collective communication operations for efficient communication among multiple processes.
4. Design your program to scale well with an increasing number of processes, ensuring that each process has enough work to do without waiting for others.
5. Implement load balancing to distribute work evenly among processes, avoiding situations where some processes are idle while others are overloaded.

### Examples

#### C/C++ MPI

1. Install an MPI implementation, such as OpenMPI or MPICH.
2. Compile your C/C++ MPI program using the provided wrapper scripts:
   - For C: `mpicc mpi_program.c -o mpi_program`
   - For C++: `mpiCC mpi_program.cpp -o mpi_program`
3. Run your MPI program using the provided `mpiexec` or `mpirun` command:
   - `mpiexec -n <number_of_processes> ./mpi_program`
   - `mpirun -n <number_of_processes> ./mpi_program`


The following C/C++ examples demonstrate different aspects of MPI:

* [MPI Hello World](https://github.com/wesleykendall/mpitutorial/blob/gh-pages/tutorials/mpi-hello-world/code/mpi_hello_world.c)
* [MPI Send and Receive](https://github.com/wesleykendall/mpitutorial/blob/gh-pages/tutorials/mpi-send-and-receive/code/send_recv.c)
* [MPI Broadcast](https://github.com/wesleykendall/mpitutorial/blob/gh-pages/tutorials/mpi-broadcast/code/bcast.c)
* [MPI Scatter and Gather](https://github.com/wesleykendall/mpitutorial/blob/gh-pages/tutorials/mpi-scatter-gather-and-allgather/code/scatter_gather.c)

#### Python MPI

1. Install the `mpi4py` library and an MPI implementation, such as OpenMPI or MPICH.
2. Run your Python MPI program using the provided `mpiexec` or `mpirun` command:
   - `mpiexec -n <number_of_processes> python mpi_program.py`
   - `mpirun -n <number_of_processes> python mpi_program.py`

The following Python examples demonstrate different aspects of MPI:

* [MPI Hello World](https://mpi4py.readthedocs.io/en/stable/tutorial.html#mpi-hello-world)
* [MPI Send and Receive](https://mpi4py.readthedocs.io/en/stable/tutorial.html#point-to-point-communication)
* [MPI Broadcast](https://mpi4py.readthedocs.io/en/stable/tutorial.html#collective-communication)
* [MPI Scatter and Gather](https://mpi4py.readthedocs.io/en/stable/tutorial.html#scatter-and-gather)
