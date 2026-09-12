## Message Passing Interface (MPI)

The **Message Passing Interface (MPI)** is a standardized, portable interface for message passing across a wide range of parallel-computing systems. It provides library routines for building parallel applications whose processes communicate by explicitly sending and receiving data. The MPI standard defines C and Fortran language bindings; C++ programs commonly use the C interface from C++ code.

MPI is central to high-performance computing (HPC) and is widely used in scientific computing, engineering simulations, and other data-intensive workloads. It supports point-to-point, collective, and other communication patterns, making it suitable for many scalable parallel algorithms.

Main idea:

- In MPI, multiple processes execute their own code independently and communicate by explicitly sending and receiving messages. This model differs from shared-memory models where processes communicate by accessing shared variables.
- MPI is defined by a standard that specifies the syntax and semantics of library routines. This ensures that MPI programs can be ported across different parallel computing platforms with minimal changes.
- MPI implementations are optimized for the underlying hardware, providing high performance while maintaining portability. This allows applications to scale from small clusters to supercomputers.
- Although MPI defines a large API, many applications rely on a relatively small core of routines for initialization, communication, coordination, and finalization.
- MPI can be used with various programming languages and supports both single-program multiple-data (SPMD) and multiple-program multiple-data (MPMD) models.

### MPI Programming Model

MPI follows a **message-passing programming model** in which independent processes exchange data explicitly. The main parts of this model are:

#### Process Model

- In a typical MPI program, the number of processes is chosen when the job starts. Each process runs independently and may be placed on a separate core, share a node with other processes, or run on another node. MPI also supports dynamic process management, though it is less common.
- Each process has a unique identifier called a **rank**, which ranges from `0` to `N-1`, where `N` is the total number of processes.
- A communicator defines a group of processes that can communicate with each other. The default communicator, `MPI_COMM_WORLD`, includes all processes.

#### Communication Types

- **Point-to-Point Communication** involves direct communication between pairs of processes. Functions like `MPI_Send` and `MPI_Recv` are used for sending and receiving messages.
- **Collective Communication** involves communication among all processes in a communicator. Functions like `MPI_Bcast`, `MPI_Reduce`, and `MPI_Barrier` are used for collective operations.
- **Non-Blocking Communication** allows processes to initiate communication operations that can proceed concurrently with computation. Functions like `MPI_Isend` and `MPI_Irecv` initiate non-blocking operations.

#### Synchronization

- `MPI_Barrier` is an explicit synchronization point: every process in the communicator must enter the barrier before any of them can return from it.
- Other collective operations such as `MPI_Bcast`, `MPI_Reduce`, and `MPI_Gather` require matching participation from the relevant processes, but they are **not** portable substitutes for a barrier. A collective call may return before other processes have completed, or even entered, the same operation.
- For parallel I/O, MPI-IO provides operations for coordinating access and making file updates visible. `MPI_File_sync`, for example, synchronizes a process's MPI file handle with the storage system, but it is not itself a communicator-wide barrier.
- Point-to-point operations such as `MPI_Sendrecv`, `MPI_Isend`, and `MPI_Irecv` let programs express more precise communication dependencies without forcing every process to synchronize at the same point.
- Synchronization is necessary for correctness, but unnecessary barriers or tightly coupled communication can leave faster processes idle and reduce parallel efficiency.

#### Process Topologies

- MPI supports the creation of virtual topologies, allowing processes to be mapped onto structured logical grids. These topologies do not affect how processes are scheduled on physical processors but influence communication patterns within the application, improving efficiency when the communication aligns with the problem’s structure.
- The `MPI_Cart_create` function helps create Cartesian topologies, which are useful for problems that naturally align with grid-based domains, such as finite difference or finite element simulations. Cartesian grids allow processes to easily determine their neighbors using MPI’s topology-aware functions, such as `MPI_Cart_shift`.
- For problems that involve irregular communication patterns, MPI provides graph topologies via `MPI_Graph_create`. This allows the programmer to define arbitrary connections between processes, which is especially useful for applications with complex or non-uniform communication requirements, such as molecular dynamics or mesh refinement algorithms.
- Using virtual topologies can enhance communication efficiency by reducing the need for complex address calculations or irregular data exchanges. MPI’s built-in functions, like `MPI_Cart_coords` and `MPI_Cart_rank`, simplify the management of communication, allowing for more natural mappings of processes to computational domains.
- MPI’s Cartesian topology support includes features such as dimensionality and periodic boundary conditions. This allows programmers to map processes in ways that reflect the physical properties of the problem, such as simulating toroidal or cylindrical systems where edge processes need to communicate with processes on the opposite side of the grid.

### Implementing Parallel Algorithms with MPI

MPI can support many parallel algorithms, but good designs usually depend on two choices: how work is divided and how processes communicate.

#### Data Decomposition

- In **domain decomposition**, the data is split into smaller, manageable chunks, where each process works on a specific portion of the dataset, which is particularly useful in large-scale **simulations** or numerical computations.
- **Task decomposition** involves dividing the overall computational tasks so that each process can work on a distinct function or subtask, often used to enhance the **parallel efficiency** of complex systems.
- Domain decomposition can support effective **load balancing** when the domain is partitioned so that processes receive comparable amounts of work. Poor partitioning can still leave some processes overloaded while others sit idle.
- In contrast, task decomposition can introduce a certain level of **heterogeneity** in workload since different tasks might have different computational demands.

#### Communication Patterns

- **Nearest neighbor communication** is a pattern where each process exchanges data only with its adjacent processes, which is particularly relevant in **grid-based computations** such as finite difference methods in scientific computing.
- **Global communication** involves operations whose result or data distribution spans an entire communicator, such as global reductions or broadcasts.
- Nearest-neighbor communication is more **localized**, so each process exchanges data with only a small set of peers. This often scales well for structured spatial domains.
- Global collectives generally require broader coordination and data movement than neighbor exchanges. Their cost can therefore become significant at large process counts, although they do not necessarily use all-to-all communication or act as barriers.

### MPI Basics

MPI has a large API, but many applications can be built from a small core of routines for initialization, process discovery, communication, and finalization.

#### Core MPI Functions

| **Function**          | **Description**                                                                    | **Parameters**                                                     |
|-----------------------|------------------------------------------------------------------------------------|--------------------------------------------------------------------|
| **MPI_Init**          | Initializes the classic MPI execution environment. It must be called before most MPI routines in this model. | `int *argc, char ***argv` - Arguments passed to the program        |
| **MPI_Finalize**      | Finalizes the classic MPI execution environment after MPI work is complete. | None                                                               |
| **MPI_Comm_size**     | Determines the size of the group associated with a communicator.                    | `MPI_Comm comm, int *size` - Communicator and pointer to store size|
| **MPI_Comm_rank**     | Determines the rank of the calling process in the communicator.                     | `MPI_Comm comm, int *rank` - Communicator and pointer to store rank|
| **MPI_Send**          | Performs a standard-mode, blocking send.                                            | `void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm` |
| **MPI_Recv**          | Performs a standard-mode, blocking receive.                                         | `void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status` |

#### Introduction to MPI Communicators

- A **communicator** in MPI refers to a fundamental concept that represents a group of processes capable of exchanging messages with each other, forming the basis for coordinated communication in parallel programs.
- **MPI_COMM_WORLD** is the default communicator provided by MPI, which encompasses all the processes that are spawned when the MPI program begins, enabling broad communication across the entire set of processes.
- MPI supports the creation of new communicators to organize and define **process subgroups**, offering flexibility for modular programming and enabling the isolation of communication within different parts of the program, such as in libraries.
- Communicators also establish a **context**, which acts as a secure communication domain, ensuring that messages transmitted within one communicator are not mistakenly received by processes in another, thus preventing message conflicts when using multiple libraries or modular components in a program.

#### Example Program: "Hello World" in MPI

Below is a simple MPI program that illustrates the basic structure of an MPI application.

##### C Version

```c

#include <mpi.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;

    // Initialize the MPI environment

    MPI_Init(&argc, &argv);

    // Get the number of processes

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the rank of the process

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Print off a hello world message
    printf("Hello world from rank %d out of %d processors\n", rank, size);

    // Finalize the MPI environment

    MPI_Finalize();

    return 0;

}

```

##### Fortran Version

```fortran
program hello_world
    use mpi
    implicit none

    integer :: rank, size, ierr

    ! Initialize the MPI environment
    call MPI_Init(ierr)

    ! Get the number of processes
    call MPI_Comm_size(MPI_COMM_WORLD, size, ierr)

    ! Get the rank of the process
    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

    ! Print off a hello world message
    print *, 'Hello world from rank', rank, 'out of', size, 'processors'

    ! Finalize the MPI environment
    call MPI_Finalize(ierr)
end program hello_world

```

##### Compilation and Execution

To compile the C version:

```bash
mpicc hello_world.c -o hello_world

```

To compile the Fortran version:

```bash
mpif90 hello_world.f90 -o hello_world

```

To execute the program with 4 processes:

```bash
mpirun -np 4 ./hello_world

```

Sample Output:

```

Hello world from rank 0 out of 4 processors

Hello world from rank 1 out of 4 processors

Hello world from rank 2 out of 4 processors

Hello world from rank 3 out of 4 processors

```

### Sending and Receiving Messages

MPI provides various communication functions to send and receive messages between processes.

#### MPI_Send and MPI_Recv

- `MPI_Send` performs a standard-mode, blocking send from one process to another.
- When `MPI_Send` returns, the sender may safely reuse the send buffer. This does **not** necessarily mean that the destination process has already received the message; MPI may use internal buffering.
- `MPI_Recv` blocks until a matching message is available and has been copied into the receive buffer.
- The receive buffer specifies a maximum count. The actual number of received elements may be smaller and can be determined from the returned `MPI_Status`.

#### Example: Sending Messages Between Processes

Consider an example where process 0 sends a message to process 1.

##### C Version

```c

#include <mpi.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size, number;

    MPI_Status status;

    MPI_Init(&argc, &argv);               // Initialize MPI environment

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank

    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get size

    if (rank == 0) {

        // Process 0
        number = 42; // Some arbitrary data

        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Process 0 sent number %d to process 1\n", number);

    } else if (rank == 1) {

        // Process 1

        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Process 1 received number %d from process 0\n", number);

    }

    MPI_Finalize();
    return 0;

}

```

##### Fortran Version

```fortran
program send_recv_example
    use mpi
    implicit none

    integer :: rank, size, number, ierr, status(MPI_STATUS_SIZE)

    call MPI_Init(ierr)
    call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
    call MPI_Comm_size(MPI_COMM_WORLD, size, ierr)

    if (rank == 0) then
        number = 42
        call MPI_Send(number, 1, MPI_INTEGER, 1, 0, MPI_COMM_WORLD, ierr)
        print *, 'Process 0 sent number', number, 'to process 1'
    else if (rank == 1) then
        call MPI_Recv(number, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, status, ierr)
        print *, 'Process 1 received number', number, 'from process 0'
    end if

    call MPI_Finalize(ierr)
end program send_recv_example

```

##### Output

```

Process 0 sent number 42 to process 1

Process 1 received number 42 from process 0

```

### Non-Blocking Communication

Non-blocking communication lets a process initiate an operation and continue before that operation is complete. This can make it possible to overlap communication with useful computation, provided the MPI implementation and workload allow communication progress while the process continues working.

| **Function**          | **Description**                                                                | **Parameters**                                                                                      |
|-----------------------|--------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------|
| **MPI_Isend**         | Initiates a non-blocking send.                                                  | `void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request`|
| **MPI_Irecv**         | Initiates a non-blocking receive.                                               | `void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request`|
| **MPI_Wait**          | Waits for a non-blocking operation to complete.                                 | `MPI_Request *request, MPI_Status *status`                                                          |

#### Example: Non-Blocking Communication

```c

#include <mpi.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size, number;

    MPI_Request request;

    MPI_Status status;

    MPI_Init(&argc, &argv);               // Initialize MPI environment

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank

    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get size

    if (rank == 0) {
        number = 42;

        MPI_Isend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);

        // Do some computation here

        MPI_Wait(&request, &status); // Ensure send is complete
        printf("Process 0 sent number %d to process 1\n", number);

    } else if (rank == 1) {

        MPI_Irecv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);

        // Do some computation here

        MPI_Wait(&request, &status); // Ensure receive is complete
        printf("Process 1 received number %d from process 0\n", number);

    }

    MPI_Finalize();
    return 0;

}

```

### Collective Communication

Collective communication involves all processes in a communicator. MPI provides various collective operations such as:

| **Function**          | **Description**                                                                | **Parameters**                                                                                                  |
|-----------------------|--------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------|
| **MPI_Bcast**         | Broadcasts a message from one process to all other processes.                   | `void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm`                                        |
| **MPI_Reduce**        | Performs a reduction operation (e.g., sum, max) across all processes and returns the result to a single process. | `const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm`     |
| **MPI_Allreduce**     | Similar to `MPI_Reduce`, but the result is returned to all processes.            | `const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm`               |
| **MPI_Scatter**       | Distributes distinct chunks of data from one process to all processes.           | `const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm` |
| **MPI_Gather**        | Gathers data from all processes to one process.                                 | `const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm` |

#### Example: MPI_Reduce

Suppose each process has a local sum, and we want to compute the global sum.

```c

#include <mpi.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size, local_sum, global_sum;

    MPI_Init(&argc, &argv);               // Initialize MPI environment

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank

    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get size

    local_sum = rank + 1; // For example, local_sum = rank + 1

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Global sum is %d\n", global_sum);

    }

    MPI_Finalize();
    return 0;

}

```

Output:

If there are 4 processes, the local sums are 1, 2, 3, 4, and the global sum is 10.

```

Global sum is 10

```

### MPI Language Bindings

Modern MPI standards define language bindings for C and Fortran. C++ applications can call the C MPI interface directly from C++ code; the older standardized C++ bindings were deprecated in MPI-2.2 and removed in MPI-3.0.

#### C Language Binding

- C programs include `mpi.h` to access MPI routines, types, constants, and handles.
- MPI routine names use the `MPI_` prefix, for example `MPI_Send` and `MPI_Comm_rank`.
- MPI routines return an integer error code; `MPI_SUCCESS` indicates successful completion when the configured MPI error handler returns errors to the caller.
- Communicators, datatypes, requests, and other MPI objects are represented through MPI-defined handle types.
- MPI datatypes such as `MPI_INT` and `MPI_FLOAT` describe the types of elements transferred in messages.

#### Fortran Language Binding

- Modern Fortran MPI code should generally prefer `use mpi_f08` when the implementation supports it. The older `use mpi` module and legacy `include 'mpif.h'` interface are also widely encountered.
- Fortran source is case-insensitive, so MPI procedure names may be written in uppercase, lowercase, or mixed case.
- Traditional Fortran MPI procedures include an `ierror`/`ierr` argument for the returned error code.
- MPI modules provide constants and handle types for communicators, datatypes, requests, and other MPI objects.
- MPI datatypes such as `MPI_INTEGER` and `MPI_REAL` correspond to Fortran data representations used in communication.

### Determinism in MPI Programs

In parallel computing, **determinism** refers to the property where a program produces the same output every time it is run with the same input, regardless of the timing of events during execution. In message-passing programming models like MPI, achieving determinism can be challenging due to the inherent nondeterminism in the arrival order of messages.

Consider a scenario where two processes, **Process A** and **Process B**, send messages to a third process, **Process C**. The relative arrival order of messages from different senders is not guaranteed; it can vary with network latency, scheduling, and system load. MPI does provide a non-overtaking guarantee for matching point-to-point messages from the same sender to the same receiver, but that ordering guarantee does not impose an order across different senders.

Ensuring that an MPI program behaves deterministically is crucial for debugging, testing, and verifying parallel applications. It is the programmer's responsibility to design the communication patterns and use MPI features appropriately to achieve determinism.

To make MPI programs deterministic, programmers can employ the following techniques:

#### 1. Specifying Message Sources

`MPI_Recv` can name a specific source or use `MPI_ANY_SOURCE` to accept a matching message from any process. When the program expects a particular sender, naming that rank directly removes ambiguity and makes the communication pattern easier to reason about.

```c

// Non-deterministic receive

MPI_Recv(buffer, count, datatype, MPI_ANY_SOURCE, tag, comm, &status);

// Deterministic receive from a specific source

MPI_Recv(buffer, count, datatype, source_rank, tag, comm, &status);

```

#### 2. Using Message Tags

MPI messages carry an integer **tag** that is matched by receive operations. Using distinct tags for different message types helps processes distinguish traffic and match the intended message explicitly.

```c

// Sender

MPI_Send(data, count, datatype, dest_rank, TAG_DATA, comm);

// Receiver

MPI_Recv(data, count, datatype, source_rank, TAG_DATA, comm, &status);

```

#### 3. Ordering Communication Operations

A fixed communication sequence can also improve determinism. When processes issue sends and receives in a well-defined order, message matching is easier to predict. Barriers may be useful between distinct phases, but they should not be added merely to compensate for an unclear communication design.

#### 4. Avoiding Wildcards

Use `MPI_ANY_SOURCE` and `MPI_ANY_TAG` only when the program genuinely accepts more than one possible sender or message type. If several messages can match, the selected message may vary between executions.

#### Example: Nondeterministic Program

The following ring example uses `MPI_ANY_SOURCE` and `MPI_ANY_TAG`, two wildcard forms that can introduce nondeterministic matching when more than one message is eligible. In this simplified ring, each receive is intended to correspond to a specific neighbor, so the main lesson is to encode that expected source and tag explicitly.

The processes are arranged in a ring. Each process sends data to a neighbor, receives data using wildcard matching, and later exchanges an additional message across the ring.

```c
#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int myid, np, rnbr, rdest;
    float buff[600];

    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    MPI_Comm_size(MPI_COMM_WORLD, &np);

    rnbr = (myid + 1) % np;
    rdest = (myid + np / 2 + 1) % np;

    // Circulate data around ring
    for (int i = 0; i < np / 2; i++) {

        MPI_Send(buff, 600, MPI_FLOAT, rnbr, 1, MPI_COMM_WORLD);

        MPI_Recv(buff, 600, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    }

    // Return accumulated data to source

    MPI_Send(buff, 300, MPI_FLOAT, rdest, 2, MPI_COMM_WORLD);

    MPI_Recv(buff, 300, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    MPI_Finalize();
    return 0;

}

```

In this program:

- Wildcard receives make the receive pattern less explicit. They become nondeterministic when several pending messages could satisfy the same receive.
- A **message race** occurs when the program's behavior depends on which eligible message a wildcard receive matches first.
- The send-then-receive ring pattern also deserves care: because `MPI_Send` is a standard blocking send, a program must not assume that every send will be buffered. `MPI_Sendrecv` or correctly completed non-blocking operations are safer choices for symmetric exchanges that might otherwise form a cyclic wait.

#### Ensuring Determinism in the Example

To make the program deterministic, modify the `MPI_Recv` calls to specify the exact source and tag expected.

```c

int lnbr = (myid - 1 + np) % np;
MPI_Recv(buff, 600, MPI_FLOAT, lnbr, 1, MPI_COMM_WORLD, &status);

```

By specifying the expected sender and tag, the process removes the ambiguity introduced by the wildcards. In this ring, each process sends to its right neighbor, so the corresponding receive should name the **left** neighbor as its source.

### MPI Collective Communication

Parallel algorithms often need coordinated communication across a group of processes. MPI provides **collective communication functions** for common patterns such as broadcast, reduction, scatter, and gather. Using these operations usually makes intent clearer and lets the MPI implementation choose an efficient algorithm for the platform.

#### Key MPI Collective Communication Functions

Below is a summary of important collective communication functions provided by MPI:

| **Function**        | **Purpose**                                           |
|---------------------|-------------------------------------------------------|
| `MPI_Barrier`       | Synchronizes all processes in a communicator.         |
| `MPI_Bcast`         | Broadcasts data from one process to all others.       |
| `MPI_Gather`        | Gathers data from all processes to one process.       |
| `MPI_Scatter`       | Distributes distinct data from one process to all.    |
| `MPI_Reduce`        | Reduces values on all processes to a single result.   |
| `MPI_Allreduce`     | Similar to `MPI_Reduce`, but result is shared.        |
| `MPI_Allgather`     | Gathers data from all processes to all processes.     |
| `MPI_Alltoall`      | Sends data from all processes to all processes.       |

#### Complete Example: Parallel Summation

Let's consider an example where we need to compute the sum of an array distributed across multiple processes.

Problem Description:

- Compute the global sum of a large array distributed across `N` processes.
- Each process holds a portion of the array (`local_array`).
- Each process computes the sum of its local array (`local_sum`).
- Use `MPI_Reduce` to compute the global sum (`global_sum`) at the root process.

##### Code Example (C)

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int np, me, array_size, local_size;
    double *global_array = NULL;
    double *local_array;
    double local_sum = 0.0, global_sum = 0.0;
    int root = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &np);

    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    if (me == root) {

        // Define the total size of the array
        array_size = 1000;
        global_array = malloc(array_size * sizeof(double));

        // Initialize the global array
        for (int i = 0; i < array_size; i++) {
            global_array[i] = 1.0; // Example value

        }

    }

    // Broadcast the array size to all processes

    MPI_Bcast(&array_size, 1, MPI_INT, root, MPI_COMM_WORLD);

    // Determine the size of each local array
    local_size = array_size / np;

    // Allocate memory for the local array
    local_array = malloc(local_size * sizeof(double));

    // Scatter the global array to all local arrays

    MPI_Scatter(global_array, local_size, MPI_DOUBLE, local_array, local_size, MPI_DOUBLE, root, MPI_COMM_WORLD);

    // Each process computes the sum of its local array
    for (int i = 0; i < local_size; i++) {
        local_sum += local_array[i];

    }

    // Reduce all local sums to a global sum at the root process

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

    if (me == root) {
        printf("Global sum is: %f\n", global_sum);
        free(global_array);

    }

    // Clean up
    free(local_array);

    MPI_Finalize();
    return 0;

}

```

Explanation:

- The root process initializes the global array.
- The array size is broadcast to all processes so that each process can allocate its local storage.
- The global array is scattered so that each process receives one segment.
- Each process computes the sum of its local segment.
- The local sums are reduced to a global sum using `MPI_Reduce` with the `MPI_SUM` operation.
- The root process prints the global sum.
- This simplified example assumes `array_size` is evenly divisible by the number of processes. For uneven partitions, use counts and displacements with `MPI_Scatterv`.

#### Finite Difference Problem Using MPI

We aim to solve a finite difference problem where a computational domain is divided among multiple processes. The algorithm requires:

- **Nearest-neighbor communication** occurs when each process requires data from its immediate neighboring processes to perform local computations or updates, commonly seen in grid-based or iterative algorithms.
- **Global communication** is necessary when processes must collectively evaluate a condition, such as determining convergence or termination, based on a **global error metric** that involves contributions from all processes.

##### Code Example (C)

```c

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void compute(float *local, int lsize) {

    // Example computation: update local array
    for (int i = 1; i <= lsize; i++) {
        local[i] = (local[i - 1] + local[i] + local[i + 1]) / 3.0;

    }

}

float maxerror(float *local, int lsize) {
    float local_err = 0.0;
    for (int i = 1; i <= lsize; i++) {
        float err = fabs(local[i] - local[i - 1]);
        if (err > local_err) {
            local_err = err;

        }

    }
    return local_err;

}

int main(int argc, char *argv[]) {

    MPI_Comm com = MPI_COMM_WORLD;
    int np, me, size, lsize;
    float *work = NULL, *local;
    float globalerr = 99999.0, localerr;
    int lnbr, rnbr;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(com, &np);

    MPI_Comm_rank(com, &me);

    if (me == 0) {

        // Initialize problem size and data
        size = 1000;
        work = malloc(size * sizeof(float));
        const double pi = acos(-1.0);
        for (int i = 0; i < size; i++) {
            work[i] = sin(i * pi / size);

        }

    }

    // Broadcast the problem size

    MPI_Bcast(&size, 1, MPI_INT, 0, com);

    // Determine local size and allocate local array with ghost cells
    lsize = size / np;
    local = malloc((lsize + 2) * sizeof(float)); // +2 for ghost cells

    // Scatter the data to all processes

    MPI_Scatter(work, lsize, MPI_FLOAT, local + 1, lsize, MPI_FLOAT, 0, com);

    // Initialize ghost cells
    local[0] = 0.0;
    local[lsize + 1] = 0.0;

    // Determine neighbor ranks (assuming periodic boundary conditions)
    lnbr = (me == 0) ? np - 1 : me - 1;
    rnbr = (me == np - 1) ? 0 : me + 1;

    while (globalerr > 0.0001) {

        // Exchange boundary data with neighbors

        MPI_Sendrecv(&local[1], 1, MPI_FLOAT, lnbr, 0,

                     &local[lsize + 1], 1, MPI_FLOAT, rnbr, 0, com, MPI_STATUS_IGNORE);

        MPI_Sendrecv(&local[lsize], 1, MPI_FLOAT, rnbr, 0,

                     &local[0], 1, MPI_FLOAT, lnbr, 0, com, MPI_STATUS_IGNORE);

        // Compute new values
        compute(local, lsize);

        // Compute local error
        localerr = maxerror(local, lsize);

        // Compute global maximum error

        MPI_Allreduce(&localerr, &globalerr, 1, MPI_FLOAT, MPI_MAX, com);

    }

    // Gather the results at the root process

    MPI_Gather(local + 1, lsize, MPI_FLOAT, work, lsize, MPI_FLOAT, 0, com);

    if (me == 0) {

        // Process and output the results
        printf("Computation complete. Sample output:\n");
        for (int i = 0; i < 10; i++) {
            printf("work[%d] = %f\n", i, work[i]);

        }
        free(work);

    }

    free(local);

    MPI_Finalize();
    return 0;

}

```

Explanation:

- The root process initializes the problem size and data array.
- The problem size is broadcast, and the data is scattered among processes.
- Each process allocates extra space for ghost cells that hold boundary data from neighboring processes.
- Processes exchange boundary values with their left and right neighbors using `MPI_Sendrecv`.
- Each process updates its local data using the finite-difference stencil.
- Each process computes a local error, and `MPI_Allreduce` finds the maximum error across all processes.
- The loop continues until the global error falls below the chosen threshold.
- The final local arrays are gathered at the root process.
- Like the previous scatter example, this code assumes the global size is evenly divisible by the number of processes.

### MPI Modularity and Communicators

In complex parallel applications, it is essential to structure the code into modules for maintainability and reusability. MPI supports modular programming through the use of **communicators**, which define communication contexts and process groups.

#### Communicators in MPI

A **communicator** in MPI is an object that represents a group of processes that can communicate with each other. The default communicator, `MPI_COMM_WORLD`, includes all the processes in the MPI program.

#### Creating New Communicators

| **Function**          | **Description**                                                                                          | **Parameters**                                                                                      |
|-----------------------|----------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------|
| **MPI_Comm_dup**      | Duplicates an existing communicator to create a new one with the same group but a different communication context. This is useful for isolating communication in different modules. | `MPI_Comm comm, MPI_Comm *newcomm`                                                                  |
| **MPI_Comm_split**    | Splits a communicator into multiple, disjoint sub-communicators based on color and key values. This is useful for creating process subgroups. | `MPI_Comm comm, int color, int key, MPI_Comm *newcomm`                                              |

Splitting Processes into Subgroups:

```c

MPI_Comm new_comm;
int color = myid / 4; // Divide processes into groups of 4

MPI_Comm_split(MPI_COMM_WORLD, color, myid, &new_comm);

// Now new_comm contains a subgroup of processes

```

#### Communicating Between Groups

To enable communication between different groups, MPI provides **intercommunicators**.

| **Function**              | **Description**                                                                                        | **Parameters**                                                                                                      |
|---------------------------|--------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------|
| **MPI_Intercomm_create**   | Creates an intercommunicator that allows communication between two groups of processes.                | `MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm`     |

Creating an Intercommunicator:

```c

MPI_Comm intercomm;
int local_leader = 0; // Rank of leader in local group
int remote_leader_world_rank = /* rank of the other group's leader in MPI_COMM_WORLD */;

MPI_Intercomm_create(local_comm, local_leader, MPI_COMM_WORLD,
                     remote_leader_world_rank, tag, &intercomm);

// remote_leader_world_rank identifies the leader of the other group
// in the peer communicator (MPI_COMM_WORLD).

```

### MPI Derived Data Types

Data sent through MPI is not always contiguous in memory. **Derived datatypes** describe structured or strided layouts so MPI can communicate them directly without requiring the application to manually pack every element into a contiguous buffer.

| **Function**              | **Description**                                                                                      | **Parameters**                                                                                                       |
|---------------------------|------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------|
| **MPI_Type_contiguous**    | Creates a new data type representing a contiguous block of elements.                                 | `int count, MPI_Datatype oldtype, MPI_Datatype *newtype`                                                             |
| **MPI_Type_vector**        | Creates a data type representing blocks of elements with a regular stride.                           | `int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype`                                 |
| **MPI_Type_indexed**       | Creates a data type with blocks at arbitrary displacements.                                          | `int count, const int array_of_blocklengths[], const int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype` |

#### Example: Sending a Column of a Matrix

Suppose we have a 2D array stored in row-major order, and we want to send a column.

```c
int rows = 10, cols = 10;
double matrix[rows][cols];

MPI_Datatype column_type;

// Create a data type for a column

MPI_Type_vector(rows, 1, cols, MPI_DOUBLE, &column_type);

MPI_Type_commit(&column_type);

// Send the column starting at matrix[0][col_index]
int dest = 1;

MPI_Send(&matrix[0][col_index], 1, column_type, dest, tag, MPI_COMM_WORLD);

// Clean up

MPI_Type_free(&column_type);

```

Benefits:

- Avoids copying non-contiguous data into a contiguous buffer.
- Simplifies code by allowing MPI to handle complex data layouts.

### Asynchronous Communication

MPI non-blocking communication allows a process to initiate communication and continue before the operation is complete. This can enable communication/computation overlap, but actual asynchronous progress depends on the MPI implementation, configuration, and whether the program continues to enter MPI as needed.

#### Non-blocking Operations

- **MPI_Isend** and **MPI_Irecv** are used to initiate **non-blocking** send and receive operations, allowing processes to continue their execution without waiting for the communication to complete.
- **MPI_Wait** and **MPI_Test** are functions that handle the completion of non-blocking operations, with MPI_Wait pausing execution until the operation finishes, while MPI_Test checks whether the operation has completed without blocking the process.

#### Probing for Messages

Sometimes a process needs to check whether a matching message is available before receiving it.

| **Function**          | **Description**                                                                                | **Parameters**                                                                                         |
|-----------------------|------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------|
| **MPI_Iprobe**        | Non-blocking check for the arrival of a message.                                                | `int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status`                                     |
| **MPI_Probe**         | Blocking check for a message; returns when a message is available.                              | `int source, int tag, MPI_Comm comm, MPI_Status *status`                                                |

#### Example: Dynamic Message Handling

```c

MPI_Status status;
int flag;

// Periodically check for messages
while (!done) {

    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
    if (flag) {

        // Message is available, determine its size
        int count;

        MPI_Get_count(&status, MPI_DOUBLE, &count);
        double *buffer = malloc(count * sizeof(double));

        // Receive the message

        MPI_Recv(buffer, count, MPI_DOUBLE, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Process the message
        process_message(buffer, count);

        free(buffer);

    }

    // Perform other computations
    perform_computation();

}

```

Use Cases:

- Handling requests from multiple clients where the arrival order is not known.
- Processes requesting more work when they are idle.

### Best Practices 

1. Minimize communication overhead by avoiding unnecessary messages and transferring only the data that is needed.
2. Use non-blocking communication when it enables useful overlap or helps express communication dependencies clearly.
3. Prefer collective operations when the communication pattern is naturally collective; MPI implementations can optimize them for the platform.
4. Balance work across processes so that some ranks do not remain idle while others become bottlenecks.
5. Check MPI return codes when using an error handler that returns errors, and handle failures deliberately.
6. Make sure point-to-point sends and receives are matched consistently by communicator, source/destination, tag, datatype, and count.
7. Use patterns such as `MPI_Sendrecv` or carefully completed non-blocking operations to avoid common cyclic-wait deadlocks; non-blocking calls alone do not guarantee deadlock freedom.
8. Free communicators, derived datatypes, requests, and other resources when they are no longer needed.
9. Design algorithms to reduce communication frequency and volume as process counts grow.
10. Measure scalability rather than assuming that adding processes will improve performance.

### Examples

#### C/C++ MPI

1. Install an MPI implementation, such as OpenMPI or MPICH.
2. Compile your C/C++ MPI program using the provided wrapper scripts:
   - For C: `mpicc mpi_program.c -o mpi_program`
   - For C++: `mpic++ mpi_program.cpp -o mpi_program` (some implementations also provide `mpiCC`)
3. Run your MPI program using the provided `mpiexec` or `mpirun` command:
   - `mpiexec -n <number_of_processes> ./mpi_program`
   - `mpirun -n <number_of_processes> ./mpi_program`

The following C/C++ examples demonstrate different aspects of MPI:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | [hello_world_mpi.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/hello_world_mpi.cpp) | Basic MPI program demonstrating initialization, rank identification, and finalization |
| 2  | [point_to_point_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/point_to_point_communication.cpp) | Demonstrate point-to-point communication using MPI_Send and MPI_Recv |
| 3  | [broadcast_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/broadcast_communication.cpp) | Demonstrate broadcasting data from one process to all processes |
| 4  | [scatter_gather.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/scatter_gather.cpp) | Demonstrate scattering and gathering data among processes |
| 5  | [reduce_operation.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/reduce_operation.cpp) | Demonstrate reduction operations to combine data from all processes |
| 6  | [allreduce_operation.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/allreduce_operation.cpp) | Demonstrate allreduce operation combining reduce and broadcast |
| 7  | [barrier_synchronization.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/barrier_synchronization.cpp) | Synchronize all processes using MPI_Barrier |
| 8  | [non_blocking_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/non_blocking_communication.cpp) | Demonstrate non-blocking communication with MPI_Isend and MPI_Irecv |
| 9  | [asynchronous_communication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/asynchronous_communication.cpp) | Demonstrate asynchronous communication patterns in MPI |
| 10 | [sendrecv_operation.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/sendrecv_operation.cpp) | Demonstrate simultaneous send and receive with MPI_Sendrecv |
| 11 | [communicators_and_groups.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/communicators_and_groups.cpp) | Create and manage custom communicators and process groups |
| 12 | [cartesian_topology.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/cartesian_topology.cpp) | Create and use Cartesian topology for structured process communication |
| 13 | [ring_topology.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/ring_topology.cpp) | Implement ring topology communication pattern |
| 14 | [derived_data_types.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/derived_data_types.cpp) | Create and use derived data types for complex data structures |
| 15 | [parallel_computation_of_pi.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/parallel_computation_of_pi.cpp) | Compute Pi in parallel using numerical integration |
| 16 | [parallel_matrix_multiplication.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/mpi/parallel_matrix_multiplication.cpp) | Perform parallel matrix multiplication using MPI |

#### Python MPI

1. Install the `mpi4py` library and an MPI implementation, such as OpenMPI or MPICH.
2. Run your Python MPI program using the provided `mpiexec` or `mpirun` command:
   - `mpiexec -n <number_of_processes> python mpi_program.py`
   - `mpirun -n <number_of_processes> python mpi_program.py`

The following Python examples demonstrate different aspects of MPI:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | [hello_world_mpi.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/hello_world_mpi.py) | Basic MPI program demonstrating initialization, rank identification, and finalization |
| 2  | [point_to_point_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/point_to_point_communication.py) | Demonstrate point-to-point communication using send and recv |
| 3  | [broadcast.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/broadcast.py) | Demonstrate broadcasting data from one process to all processes |
| 4  | [scatter_and_gather.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/scatter_and_gather.py) | Demonstrate scattering and gathering data among processes |
| 5  | [reduce.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/reduce.py) | Demonstrate reduction operations to combine data from all processes |
| 6  | [allreduce.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/allreduce.py) | Demonstrate allreduce operation combining reduce and broadcast |
| 7  | [barrier_synchronization.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/barrier_synchronization.py) | Synchronize all processes using barrier |
| 8  | [non_blocking_communication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/non_blocking_communication.py) | Demonstrate non-blocking communication with isend and irecv |
| 9  | [sendrecv.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/sendrecv.py) | Demonstrate simultaneous send and receive with sendrecv |
| 10 | [communicators_and_groups.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/communicators_and_groups.py) | Create and manage custom communicators and process groups |
| 11 | [cartesian_topology.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/cartesian_topology.py) | Create and use Cartesian topology for structured process communication |
| 12 | [ring_topology.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/ring_topology.py) | Implement ring topology communication pattern |
| 13 | [parallel_computation_of_pi.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/parallel_computation_of_pi.py) | Compute Pi in parallel using numerical integration |
| 14 | [parallel_matrix_multiplication.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/mpi/parallel_matrix_multiplication.py) | Perform parallel matrix multiplication using MPI |
