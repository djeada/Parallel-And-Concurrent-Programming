# Message Passing Interface (MPI)

The **Message Passing Interface (MPI)** is a standardized and portable message-passing system designed to function on a wide variety of parallel computing architectures. It provides a set of library routines that can be called from programming languages like C, C++, and Fortran to write parallel applications. MPI allows multiple processes to communicate with one another by sending and receiving messages, enabling the development of scalable and efficient parallel programs.

MPI is central to high-performance computing (HPC) and is widely used in scientific computing, engineering simulations, and data-intensive tasks. It provides a rich set of functionalities that support both point-to-point and collective communication, making it suitable for a broad range of parallel algorithms.

Main idea:

- **Message-Passing Programming**: In MPI, multiple processes execute their own code independently and communicate by explicitly sending and receiving messages. This model differs from shared-memory models where processes communicate by accessing shared variables.

- **Standardization**: MPI is defined by a standard that specifies the syntax and semantics of library routines. This ensures that MPI programs can be ported across different parallel computing platforms with minimal changes.

- **Portability and Performance**: MPI implementations are optimized for the underlying hardware, providing high performance while maintaining portability. This allows applications to scale from small clusters to supercomputers.

- **Rich Functionality**: While MPI consists of over 100 functions, a small subset is sufficient for most applications. This subset includes functions for initialization, communication, synchronization, and finalization.

- **Interoperability**: MPI can be used with various programming languages and supports both single-program multiple-data (SPMD) and multiple-program multiple-data (MPMD) models.

## MPI Programming Model

MPI follows the **message-passing programming model**, where processes communicate by explicitly sending and receiving messages. Here are the key aspects of the MPI programming model:

### Process Model

- **Fixed Number of Processes**: In an MPI program, the number of processes is typically fixed at the start of the program. Each process runs independently and can be mapped to a separate processor or core.

- **Rank**: Each process has a unique identifier called a **rank**, which ranges from `0` to `N-1`, where `N` is the total number of processes.

- **Communicators**: A communicator defines a group of processes that can communicate with each other. The default communicator, `MPI_COMM_WORLD`, includes all processes.

### Communication Types

- **Point-to-Point Communication** involves direct communication between pairs of processes. Functions like `MPI_Send` and `MPI_Recv` are used for sending and receiving messages.

- **Collective Communication** involves communication among all processes in a communicator. Functions like `MPI_Bcast`, `MPI_Reduce`, and `MPI_Barrier` are used for collective operations.

- **Non-Blocking Communication** allows processes to initiate communication operations that can proceed concurrently with computation. Functions like `MPI_Isend` and `MPI_Irecv` initiate non-blocking operations.

### Synchronization

- **Barrier Synchronization**: MPI provides the `MPI_Barrier` function, which acts as a synchronization point. All processes in a communicator must reach this barrier before any can proceed. This ensures that no process moves forward until all others have caught up, making it useful for coordinating stages of parallel computation.
  
- **Collective Operations**: In addition to `MPI_Barrier`, other collective operations like `MPI_Bcast`, `MPI_Reduce`, and `MPI_Gather` implicitly involve synchronization. While these functions perform data movement, they also ensure synchronization by requiring participation from all processes within a communicator.
  
- **Synchronization for I/O**: When performing parallel I/O (e.g., using MPI-IO), synchronization is essential to ensure data consistency and ordering. MPI provides various mechanisms for synchronized I/O operations, such as `MPI_File_sync` to ensure all changes are flushed to disk.

- **Fine-Grained Control**: MPI allows for finer synchronization via point-to-point communication calls like `MPI_Sendrecv` and non-blocking functions (`MPI_Isend`, `MPI_Irecv`). These allow more precise control over synchronization by managing communication dependencies without forcing all processes to wait.

- **Performance Considerations**: While synchronization ensures correctness in parallel programs, excessive use of barriers or collective synchronization functions can hurt performance by forcing idle times for processes. Minimizing unnecessary synchronization is important for optimizing parallel efficiency.

### Process Topologies

- MPI supports the creation of virtual topologies, allowing processes to be mapped onto structured logical grids. These topologies do not affect how processes are scheduled on physical processors but influence communication patterns within the application, improving efficiency when the communication aligns with the problem’s structure.
- The `MPI_Cart_create` function helps create Cartesian topologies, which are useful for problems that naturally align with grid-based domains, such as finite difference or finite element simulations. Cartesian grids allow processes to easily determine their neighbors using MPI’s topology-aware functions, such as `MPI_Cart_shift`.
- For problems that involve irregular communication patterns, MPI provides graph topologies via `MPI_Graph_create`. This allows the programmer to define arbitrary connections between processes, which is especially useful for applications with complex or non-uniform communication requirements, such as molecular dynamics or mesh refinement algorithms.

- Using virtual topologies can enhance communication efficiency by reducing the need for complex address calculations or irregular data exchanges. MPI’s built-in functions, like `MPI_Cart_coords` and `MPI_Cart_rank`, simplify the management of communication, allowing for more natural mappings of processes to computational domains.

- **Dimensionality and Periodicity**: MPI’s Cartesian topology support includes features such as dimensionality and periodic boundary conditions. This allows programmers to map processes in ways that reflect the physical properties of the problem, such as simulating toroidal or cylindrical systems where edge processes need to communicate with processes on the opposite side of the grid.



## Implementing Parallel Algorithms with MPI

MPI provides the tools necessary to implement a wide range of parallel algorithms. Here are some considerations:

### Data Decomposition

- In **domain decomposition**, the data is split into smaller, manageable chunks, where each process works on a specific portion of the dataset, which is particularly useful in large-scale **simulations** or numerical computations.
- **Task decomposition** involves dividing the overall computational tasks so that each process can work on a distinct function or subtask, often used to enhance the **parallel efficiency** of complex systems.
- The primary benefit of domain decomposition is that it allows for **load balancing**, ensuring that each process is equally utilized, minimizing idle time and maximizing computational resources.
- In contrast, task decomposition can introduce a certain level of **heterogeneity** in workload since different tasks might have different computational demands.

### Communication Patterns

- **Nearest neighbor communication** is a pattern where each process exchanges data only with its adjacent processes, which is particularly relevant in **grid-based computations** such as finite difference methods in scientific computing.
- **Global communication** involves scenarios where processes need to exchange data with every other process, such as in collective operations like **global sums**, reductions, or broadcasts.
- Nearest neighbor communication is more **localized**, reducing the communication overhead since data only needs to travel to nearby processes, making it efficient in systems with a structured spatial domain.
- On the other hand, global communication often involves **all-to-all communication**, requiring synchronization across all processes, which can introduce **latency** and bottlenecks in large-scale systems.

## MPI Basics

While MPI provides a comprehensive set of functions, many parallel applications can be developed using a core subset of functions. These functions cover initialization, communication, and finalization.

### Core MPI Functions

| **Function**          | **Description**                                                                    | **Parameters**                                                     |
|-----------------------|------------------------------------------------------------------------------------|--------------------------------------------------------------------|
| **MPI_Init**          | Initializes the MPI execution environment. Must be called before any other MPI function. | `int *argc, char ***argv` - Arguments passed to the program        |
| **MPI_Finalize**      | Terminates the MPI execution environment. No MPI functions can be called after this. | None                                                               |
| **MPI_Comm_size**     | Determines the size of the group associated with a communicator.                    | `MPI_Comm comm, int *size` - Communicator and pointer to store size|
| **MPI_Comm_rank**     | Determines the rank of the calling process in the communicator.                     | `MPI_Comm comm, int *rank` - Communicator and pointer to store rank|
| **MPI_Send**          | Performs a standard-mode, blocking send.                                            | `void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm` |
| **MPI_Recv**          | Performs a standard-mode, blocking receive.                                         | `void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status` |

## Introduction to MPI Communicators

- A **communicator** in MPI refers to a fundamental concept that represents a group of processes capable of exchanging messages with each other, forming the basis for coordinated communication in parallel programs.
- **MPI_COMM_WORLD** is the default communicator provided by MPI, which encompasses all the processes that are spawned when the MPI program begins, enabling broad communication across the entire set of processes.
- MPI supports the creation of new communicators to organize and define **process subgroups**, offering flexibility for modular programming and enabling the isolation of communication within different parts of the program, such as in libraries.
- Communicators also establish a **context**, which acts as a secure communication domain, ensuring that messages transmitted within one communicator are not mistakenly received by processes in another, thus preventing message conflicts when using multiple libraries or modular components in a program.

## MPI Initialization and Finalization

### MPI_Init

- Must be called once by each process before any other MPI function.

- Typically called at the beginning of the `main` function.

- Initializes the MPI environment and sets up the necessary resources.

### MPI_Finalize

- Must be called by each process after all MPI operations are completed.

- Cleans up the MPI environment and frees resources.

## Determining Process Information

### MPI_Comm_size

- Used to determine the total number of processes participating in the communicator.

- Essential for writing code that adapts to different numbers of processes.

### MPI_Comm_rank

- Used to determine the rank (identifier) of the calling process within the communicator.

- The rank is used to differentiate the behavior of processes, for example, assigning different tasks or data partitions.

## Example Program: "Hello World" in MPI

Below is a simple MPI program that illustrates the basic structure of an MPI application.

### C Version

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

### Fortran Version

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

### Compilation and Execution

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

### Sample Output

```
Hello world from rank 0 out of 4 processors
Hello world from rank 1 out of 4 processors
Hello world from rank 2 out of 4 processors
Hello world from rank 3 out of 4 processors
```

## Sending and Receiving Messages

MPI provides various communication functions to send and receive messages between processes.

### MPI_Send and MPI_Recv

- **MPI_Send** is a function used to transmit messages from one process to another in an MPI program, allowing for explicit data exchange between processes.
- The operation of MPI_Send is **blocking**, meaning that the function only completes and returns when the data in the send buffer has been safely sent, allowing the buffer to be reused for other tasks.
- **MPI_Recv** is designed to receive messages sent by other processes, facilitating communication by transferring data into the designated receive buffer.
- Like MPI_Send, MPI_Recv is also a **blocking** operation, meaning the function completes only when the receive buffer is fully populated with the incoming message, ensuring that the data is ready for processing.

### Example: Sending Messages Between Processes

Consider an example where process 0 sends a message to process 1.

#### C Version

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

#### Fortran Version

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

#### Output

```
Process 0 sent number 42 to process 1
Process 1 received number 42 from process 0
```

## Non-Blocking Communication

Non-blocking communication allows processes to initiate communication operations and then proceed without waiting for them to complete. This can be useful for overlapping computation with communication.

| **Function**          | **Description**                                                                | **Parameters**                                                                                      |
|-----------------------|--------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------|
| **MPI_Isend**         | Initiates a non-blocking send.                                                  | `void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request`|
| **MPI_Irecv**         | Initiates a non-blocking receive.                                               | `void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request`|
| **MPI_Wait**          | Waits for a non-blocking operation to complete.                                 | `MPI_Request *request, MPI_Status *status`                                                          |

### Example: Non-Blocking Communication

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

## Collective Communication

Collective communication involves all processes in a communicator. MPI provides various collective operations such as:

| **Function**          | **Description**                                                                | **Parameters**                                                                                                  |
|-----------------------|--------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------|
| **MPI_Bcast**         | Broadcasts a message from one process to all other processes.                   | `void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm`                                        |
| **MPI_Reduce**        | Performs a reduction operation (e.g., sum, max) across all processes and returns the result to a single process. | `const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm`     |
| **MPI_Allreduce**     | Similar to `MPI_Reduce`, but the result is returned to all processes.            | `const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm`               |
| **MPI_Scatter**       | Distributes distinct chunks of data from one process to all processes.           | `const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm` |
| **MPI_Gather**        | Gathers data from all processes to one process.                                 | `const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm` |


### Example: MPI_Reduce

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

### Output

If there are 4 processes, the local sums are 1, 2, 3, 4, and the global sum is 10.

```
Global sum is 10
```

## MPI Language Bindings

MPI provides language bindings for C, C++, and Fortran, allowing MPI functions to be used naturally in these languages.

### C Language Binding

- The **header file** required for MPI programming in C is `mpi.h`, which must be included in all MPI-related programs.
- MPI **function naming** follows a convention where function names are prefixed with `MPI_` and are written in CamelCase, maintaining consistency across the library.
- For **error handling**, MPI functions return an integer error code, where `MPI_SUCCESS` signifies that the operation completed successfully without any errors.
- **Constants and handles** used in MPI, such as communicators and data types, are predefined in `mpi.h`, ensuring standardized communication.
- MPI provides **data types** that correspond to C data types, such as `MPI_INT` for integers and `MPI_FLOAT` for floating-point numbers, allowing easy mapping between MPI and C.

### Fortran Language Binding

- In Fortran, either `use mpi` (for Fortran 90 and later) or `include 'mpif.h'` (for Fortran 77) must be used to access MPI functions and definitions.
- MPI **function naming** in Fortran uses uppercase letters, following a different convention compared to C.
- **Error handling** in Fortran includes an additional `ierr` parameter at the end of each function call, where the error code is stored, similar to the return value in C.
- **Constants and handles** in Fortran are defined in `mpif.h` or the `mpi` module, providing a consistent set of identifiers across MPI programs.
- MPI provides **data types** in Fortran, such as `MPI_INTEGER` for integers and `MPI_REAL` for real numbers, mapping directly to Fortran's native types.

## Example: Parallel Computation of Pi

As a more illustrative example, let's consider computing the value of Pi using numerical integration in parallel.

Algorithm:

- **Approach**: Use the numerical integration of the function \( f(x) = \frac{4}{1 + x^2} \) from \( 0 \) to \( 1 \).

- **Parallelization**: Divide the interval among processes; each process computes a partial sum over its subinterval.

- **Reduction**: Use `MPI_Reduce` to sum up the partial results.

### C Version

```c
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
```

### Explanation

- **Process distribution** is handled such that each process computes the sum over intervals, where each process begins at its rank and calculates over every `size`-th interval, distributing the workload evenly.
- **MPI_Reduce** is used to collect and sum all the local sums from individual processes, resulting in the final aggregated approximation of Pi.

# Determinism in MPI Programs

In parallel computing, **determinism** refers to the property where a program produces the same output every time it is run with the same input, regardless of the timing of events during execution. In message-passing programming models like MPI, achieving determinism can be challenging due to the inherent nondeterminism in the arrival order of messages.

Consider a scenario where two processes, **Process A** and **Process B**, send messages to a third process, **Process C**. The arrival order of these messages at Process C is not guaranteed because it depends on various factors such as network latency, scheduling, and system load. Although MPI guarantees that messages sent from one process to another are received in the order they were sent, this guarantee does not extend across multiple sender processes.

Ensuring that an MPI program behaves deterministically is crucial for debugging, testing, and verifying parallel applications. It is the programmer's responsibility to design the communication patterns and use MPI features appropriately to achieve determinism.

## Techniques for Ensuring Determinism

To make MPI programs deterministic, programmers can employ the following techniques:

### 1. Specifying Message Sources

By default, when a process calls `MPI_Recv`, it can specify the source of the message or accept messages from any source by using `MPI_ANY_SOURCE`. To ensure determinism, it is advisable to specify the exact source process from which to receive messages. This eliminates ambiguity about which message is received and in what order.

**Example**:

```c
// Non-deterministic receive
MPI_Recv(buffer, count, datatype, MPI_ANY_SOURCE, tag, comm, &status);

// Deterministic receive from a specific source
MPI_Recv(buffer, count, datatype, source_rank, tag, comm, &status);
```

### 2. Using Message Tags

MPI allows messages to be labeled with a **tag**, an integer value specified during send and receive operations. By carefully assigning and matching tags, processes can distinguish between different types of messages and ensure that they receive the correct message at the correct time.

**Example**:

```c
// Sender
MPI_Send(data, count, datatype, dest_rank, TAG_DATA, comm);

// Receiver
MPI_Recv(data, count, datatype, source_rank, TAG_DATA, comm, &status);
```

### 3. Ordering Communication Operations

Designing the communication sequence so that all processes follow a predetermined order can help in achieving determinism. This often involves structuring the program such that all sends and receives occur in a fixed sequence, possibly by using barriers or other synchronization mechanisms.

### 4. Avoiding Wildcards

Minimize the use of wildcards like `MPI_ANY_SOURCE` and `MPI_ANY_TAG` in receive operations. While they provide flexibility, they can lead to nondeterministic behavior because the order in which messages are received can vary between executions.

## Example: Nondeterministic Program

Let's examine a program that demonstrates nondeterministic behavior due to the use of `MPI_ANY_SOURCE` and `MPI_ANY_TAG`.

### Description

The program implements a symmetric pairwise interaction algorithm in which processes are arranged in a ring topology. Each process sends data halfway around the ring and then receives data from any source. Finally, it returns accumulated data to the originating process.

### C Version

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

- **Non-determinism arises** in MPI when processes use `MPI_ANY_SOURCE` and `MPI_ANY_TAG` in their `MPI_Recv` calls, allowing them to receive messages from any source or with any tag, which can lead to variability in the order messages are processed if multiple are pending.
- **Race conditions** occur when multiple messages are sent to a process, and the order in which they are received may vary between different executions, potentially causing unpredictable behavior.
- **Deadlocks** can emerge when incorrect assumptions are made about the **message ordering**, leading processes to wait indefinitely for messages that may have been received in a different sequence than expected.

### Ensuring Determinism in the Example

To make the program deterministic, modify the `MPI_Recv` calls to specify the exact source and tag expected.

**Modified Receive Call**:

```c
MPI_Recv(buff, 600, MPI_FLOAT, rnbr, 1, MPI_COMM_WORLD, &status);
```

By specifying `rnbr` as the source and `1` as the tag, the process ensures that it receives the expected message from its right neighbor with the correct tag.

## MPI Collective Communication

Parallel algorithms often require coordinated communication among multiple processes. MPI provides a set of **collective communication functions** that are optimized for such operations. These functions simplify the code and can offer performance benefits due to underlying optimizations.

### Key MPI Collective Communication Functions

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

### Complete Example: Parallel Summation

Let's consider an example where we need to compute the sum of an array distributed across multiple processes.

#### Problem Description

- Compute the global sum of a large array distributed across `N` processes.
- Each process holds a portion of the array (`local_array`).
- Each process computes the sum of its local array (`local_sum`).
- Use `MPI_Reduce` to compute the global sum (`global_sum`) at the root process.

#### Code Example (C)

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

- **Initialization**: The root process initializes the global array.
- **Broadcast**: The array size is broadcasted to all processes to ensure they allocate the correct amount of memory.
- **Scatter**: The global array is scattered so that each process receives a segment.
- **Computation**: Each process computes the sum of its local segment.
- **Reduction**: The local sums are reduced to a global sum using `MPI_Reduce` with the `MPI_SUM` operation.
- **Result**: The root process prints the global sum.

## Finite Difference Problem Using MPI

### Problem Description

We aim to solve a finite difference problem where a computational domain is divided among multiple processes. The algorithm requires:

- **Nearest-neighbor communication** occurs when each process requires data from its immediate neighboring processes to perform local computations or updates, commonly seen in grid-based or iterative algorithms.
- **Global communication** is necessary when processes must collectively evaluate a condition, such as determining convergence or termination, based on a **global error metric** that involves contributions from all processes.

### Code Example (C)

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
        for (int i = 0; i < size; i++) {
            work[i] = sin(i * M_PI / size);
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

#### Explanation

- **Initialization**: The root process initializes the problem size and data array.
- **Broadcast and Scatter**: The problem size is broadcasted, and the data is scattered among processes.
- **Ghost Cells**: Each process allocates extra space for ghost cells to hold boundary data from neighbors.
- **Communication**: Processes exchange boundary data with their left and right neighbors using `MPI_Sendrecv`.
- **Computation**: Each process updates its local data based on the finite difference scheme.
- **Global Error Calculation**: All processes compute their local error, and `MPI_Allreduce` is used to find the global maximum error.
- **Termination**: The loop continues until the global error is below a specified threshold.
- **Gathering Results**: Final results are gathered at the root process.

## MPI Modularity and Communicators

### Importance of Modularity

In complex parallel applications, it is essential to structure the code into modules for maintainability and reusability. MPI supports modular programming through the use of **communicators**, which define communication contexts and process groups.

### Communicators in MPI

A **communicator** in MPI is an object that represents a group of processes that can communicate with each other. The default communicator, `MPI_COMM_WORLD`, includes all the processes in the MPI program.

#### Creating New Communicators

| **Function**          | **Description**                                                                                          | **Parameters**                                                                                      |
|-----------------------|----------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------|
| **MPI_Comm_dup**      | Duplicates an existing communicator to create a new one with the same group but a different communication context. This is useful for isolating communication in different modules. | `MPI_Comm comm, MPI_Comm *newcomm`                                                                  |
| **MPI_Comm_split**    | Splits a communicator into multiple, disjoint sub-communicators based on color and key values. This is useful for creating process subgroups. | `MPI_Comm comm, int color, int key, MPI_Comm *newcomm`                                              |


**Example: Splitting Processes into Subgroups**

```c
MPI_Comm new_comm;
int color = myid / 4; // Divide processes into groups of 4
MPI_Comm_split(MPI_COMM_WORLD, color, myid, &new_comm);

// Now new_comm contains a subgroup of processes
```

### Communicating Between Groups

To enable communication between different groups, MPI provides **intercommunicators**.

- **`MPI_Intercomm_create`**: Creates an intercommunicator that allows communication between two groups of processes.

**Example: Creating an Intercommunicator**

```c
MPI_Comm intercomm;
int local_leader = 0; // Rank of leader in local group
int remote_leader = 0; // Rank of leader in remote group

MPI_Intercomm_create(local_comm, local_leader, MPI_COMM_WORLD, remote_leader, tag, &intercomm);

// Now processes in local_comm can communicate with processes in the remote group via intercomm
```

## MPI Derived Data Types

In many applications, data to be sent or received may not be stored contiguously in memory. MPI allows the creation of **derived data types** to describe such complex memory layouts, enabling efficient communication without extra copying.

| **Function**              | **Description**                                                                                      | **Parameters**                                                                                                       |
|---------------------------|------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------|
| **MPI_Type_contiguous**    | Creates a new data type representing a contiguous block of elements.                                 | `int count, MPI_Datatype oldtype, MPI_Datatype *newtype`                                                             |
| **MPI_Type_vector**        | Creates a data type representing blocks of elements with a regular stride.                           | `int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype`                                 |
| **MPI_Type_indexed**       | Creates a data type with blocks at arbitrary displacements.                                          | `int count, const int array_of_blocklengths[], const int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype` |


### Example: Sending a Column of a Matrix

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

### Benefits

- **Performance**: Avoids copying non-contiguous data into a contiguous buffer.
- **Convenience**: Simplifies code by allowing MPI to handle complex data layouts.

## Asynchronous Communication

### Overview

Asynchronous communication allows a process to initiate a communication operation and then proceed without waiting for it to complete. This can help overlap computation and communication, improving performance.

### Non-blocking Operations

- **MPI_Isend** and **MPI_Irecv** are used to initiate **non-blocking** send and receive operations, allowing processes to continue their execution without waiting for the communication to complete.
- **MPI_Wait** and **MPI_Test** are functions that handle the completion of non-blocking operations, with MPI_Wait pausing execution until the operation finishes, while MPI_Test checks whether the operation has completed without blocking the process.
### Probing for Messages

Sometimes, a process may need to check if a message has arrived without actually receiving it.

| **Function**          | **Description**                                                                                | **Parameters**                                                                                         |
|-----------------------|------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------|
| **MPI_Iprobe**        | Non-blocking check for the arrival of a message.                                                | `int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status`                                     |
| **MPI_Probe**         | Blocking check for a message; returns when a message is available.                              | `int source, int tag, MPI_Comm comm, MPI_Status *status`                                                |


### Example: Dynamic Message Handling

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

- **Server Processes**: Handling requests from multiple clients where the arrival order is not known.
- **Dynamic Work Distribution**: Processes requesting more work when they are idle.

### Best Practices 

1. Minimize communication overhead by reducing the number and size of messages.
2. Use non-blocking communication to allow the program to continue executing while sending or receiving messages.
3. Utilize collective communication operations for efficient communication among multiple processes.
4. Ensure your program scales well with an increasing number of processes, distributing work evenly to prevent idle or overloaded processes (implement load balancing).
5. Always check the return codes of MPI functions to handle errors gracefully.
6. Ensure every `MPI_Send` has a matching `MPI_Recv` to avoid deadlocks.
7. Use non-blocking communication or `MPI_Sendrecv` to prevent processes from waiting indefinitely and avoid deadlocks.
8. Properly manage resources by allocating and freeing communicators and data types.
9. Design algorithms that scale efficiently and minimize communication frequency and volume.
10. Leverage MPI's optimized collective communication functions to improve performance.

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
