## Message Passing Interface (MPI)

The Message Passing Interface (MPI) is a standardized and portable message-passing system designed to function on parallel computing architectures. MPI allows a collection of processes to execute programs in a standard sequential language, augmented with functions for sending and receiving messages. This system is central to parallel programming and can adapt designs developed using traditional techniques.

### Key Concepts of MPI

- **Message-Passing Programming**: Involves processes communicating through library calls to send and receive messages.
- **Standardization**: MPI is the de facto standard for message-passing libraries.
- **Broad Applicability**: Techniques in MPI are applicable to other systems like p4, PVM, Express, and PARMACS.
- **Focus on Essentials**: MPI consists of 129 functions, but a subset of 24 functions covers most applications.

### MPI Programming Model

- **Process Communication**: Processes communicate by sending and receiving messages via library routines.
- **Fixed Processes**: Typically, a fixed set of processes is created at initialization, with one process per processor.
- **Multiple Programs**: Different programs can execute on different processes, referred to as the Multiple Program Multiple Data (MPMD) model.
- **Communication Types**: 
  - **Point-to-Point**: Sending messages between specific processes.
  - **Collective Operations**: Global operations like summation and broadcast.
  - **Asynchronous Communication**: MPI can probe for messages asynchronously.
- **Modular Programming**: Uses communicators to define modules that encapsulate internal communication structures.

### Implementing Parallel Algorithms with MPI

- **Direct Implementation**: Algorithms with one task per processor use point-to-point or collective communication.
- **Refinement Needed**: Dynamic task creation or concurrent task execution needs refinement for MPI.
- **Branch-and-Bound Example**: Dynamic search task creation must be refined to create a fixed set of worker processes.

### MPI Basics

While MPI is complex, many problems can be solved using just six core functions. These functions initiate and terminate computations, identify processes, and send and receive messages.

| **Function**       | **Purpose**                            | **Parameters**                                                                                   | **Returns**                              |
|--------------------|----------------------------------------|--------------------------------------------------------------------------------------------------|------------------------------------------|
| **MPI_INIT**       | Initiate an MPI computation.           | `int *argc`, `char ***argv` (required only in C language binding).                                | None                                     |
| **MPI_FINALIZE**   | Terminate an MPI computation.          | None                                                                                             | None                                     |
| **MPI_COMM_SIZE**  | Determine the number of processes in a computation. | **IN**: `comm` (communicator handle)                                                             | **OUT**: `size` (number of processes in the group).         |
| **MPI_COMM_RANK**  | Determine the identifier of the current process. | **IN**: `comm` (communicator handle)                                                             | **OUT**: `pid` (process id in the group). |
| **MPI_SEND**       | Send a message.                        | **IN**: `buf` (address of send buffer) <br> **IN**: `count` (number of elements to send) <br> **IN**: `datatype` (datatype of send buffer elements) <br> **IN**: `dest` (destination process id) <br> **IN**: `tag` (message tag) <br> **IN**: `comm` (communicator handle). | None |
| **MPI_RECV**       | Receive a message.                     | **IN**: `count` (size of receive buffer in elements) <br> **IN**: `datatype` (datatype of receive buffer elements) <br> **IN**: `source` (source process id or `MPI_ANY_SOURCE`) <br> **IN**: `tag` (message tag or `MPI_ANY_TAG`) <br> **IN**: `comm` (communicator handle) | **OUT**: `buf` (address of receive buffer) <br> **OUT**: `status` (status object). |

#### Introduction to MPI Communicators

- **Communicator Handle**: Most MPI functions (except MPI_INIT and MPI_FINALIZE) require a communicator handle. A communicator defines the process group and context for operations.
- **MPI_COMM_WORLD**: The default communicator, representing all processes involved in a computation.
- **Modular Programs**: Communicators help in identifying process subsets and ensuring that messages for different purposes are not confused.

#### MPI Initialization and Finalization

- **MPI_INIT()**: Must be called once per process to initiate an MPI computation.
- **MPI_FINALIZE()**: Shuts down an MPI computation; no MPI functions can be called afterward.

#### Determining Process Information

- **MPI_COMM_SIZE(comm, size)**: Determines the number of processes in the computation.
  - **comm**: Communicator handle.
  - **size**: Number of processes in the communicator's group.
- **MPI_COMM_RANK(comm, pid)**: Determines the unique identifier of the current process.
  - **comm**: Communicator handle.
  - **pid**: Process ID in the communicator's group.

#### Example Program

The following example program illustrates basic MPI functions. It initializes MPI, determines the number of processes and their identifiers, prints a message, and finalizes the computation.

```text
program main
begin
    MPI_INIT()  // Initialize computation
    MPI_COMM_SIZE(MPI_COMM_WORLD, count)  // Find number of processes
    MPI_COMM_RANK(MPI_COMM_WORLD, myid)  // Find my ID
    print("I am", myid, "of", count)  // Print message
    MPI_FINALIZE()  // Shut down
end
```

#### Execution and Output

- **Program Execution**: Typically started with a command like `myprog -n 4`, where `myprog` is the executable, and `-n 4` specifies four processes.
- **Sample Output**: If executed by four processes, the output may look like:
  ```text
  I am 1 of 4
  I am 3 of 4
  I am 0 of 4
  I am 2 of 4
  ```

#### Sending and Receiving Messages

- **MPI_SEND(buf, count, datatype, dest, tag, comm)**: Sends a message.
  - **buf**: Address of the send buffer.
  - **count**: Number of elements to send.
  - **datatype**: Datatype of the elements.
  - **dest**: Destination process ID.
  - **tag**: Message tag.
  - **comm**: Communicator handle.
  
- **MPI_RECV(buf, count, datatype, source, tag, comm, status)**: Receives a message.
  - **buf**: Address of the receive buffer.
  - **count**: Size of the receive buffer.
  - **datatype**: Datatype of the elements.
  - **source**: Source process ID or `MPI_ANY_SOURCE`.
  - **tag**: Message tag or `MPI_ANY_TAG`.
  - **comm**: Communicator handle.
  - **status**: Status object.

#### Example Program with MPI_SEND and MPI_RECV

This example program demonstrates the use of MPI_SEND and MPI_RECV to implement a bridge construction algorithm. Two processes execute different tasks: the first process (foundry) sends messages, and the second process (bridge) receives them.

```text
program main
begin
    MPI_INIT()  // Initialize computation
    MPI_COMM_SIZE(MPI_COMM_WORLD, count)  // Find number of processes
    if count != 2 then exit  // Must have exactly 2 processes
    MPI_COMM_RANK(MPI_COMM_WORLD, myid)  // Find my ID
    
    if myid = 0 then  // Process 0
        foundry(100)  // Execute foundry procedure
    else  // Process 1
        bridge()  // Execute bridge procedure
    endif
    
    MPI_FINALIZE()  // Shut down
end

procedure foundry(numgirders)  // Code for process 0
begin
    for i = 1 to numgirders  // Send messages
        MPI_SEND(i, 1, MPI_INT, 1, 0, MPI_COMM_WORLD)
    endfor
    i = -1  // Send shutdown message
    MPI_SEND(i, 1, MPI_INT, 1, 0, MPI_COMM_WORLD)
end

procedure bridge  // Code for process 1
begin
    MPI_RECV(msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, status)
    while msg != -1 do  // Receive messages
        use_girder(msg)  // Use received message
        MPI_RECV(msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, status)
    enddo
end
```

 Explanation
 
- **Process 0 (foundry)**: Sends 100 integer messages to Process 1 and a shutdown message (−1).
- **Process 1 (bridge)**: Receives messages from Process 0 until the shutdown message is received. 

This program demonstrates how different tasks can be coordinated using MPI_SEND and MPI_RECV within an MPI computation.

### Language Bindings

MPI supports multiple programming languages, including C and Fortran. The functions described can be used across different languages with specific bindings tailored to each language’s syntax.

#### C Language Binding
- **Function Names**: Use the MPI prefix and capitalize the first letter of the function name.
- **Return Codes**: Functions return integer codes; `MPI_SUCCESS` indicates successful completion.
- **Constants and Handles**: Defined in `mpi.h`, which must be included in any MPI program.
- **Parameter Passing**:
  - **IN Parameters**: Passed by value.
  - **OUT and INOUT Parameters**: Passed by reference (pointers).
- **Status Variables**: Defined as `MPI_Status`, a structure with fields for source (`status.MPI_SOURCE`) and tag (`status.MPI_TAG`).
- **MPI Datatypes**: Specific to C, such as `MPI_CHAR`, `MPI_INT`, `MPI_FLOAT`, etc.

#### Fortran Language Binding
- **Function Names**: Written in upper case.
- **Return Codes**: Represented by an additional integer argument; `MPI_SUCCESS` indicates successful completion.
- **Constants and Handles**: Defined in `mpif.h`, which must be included in any MPI program.
- **Handles**: Represented by `INTEGER`.
- **Status Variables**: Defined as an array of integers of size `MPI_STATUS_SIZE`, with constants `MPI_SOURCE` and `MPI_TAG` indexing the source and tag fields.
- **MPI Datatypes**: Specific to Fortran, such as `MPI_INTEGER`, `MPI_REAL`, `MPI_DOUBLE_PRECISION`, etc.

### Example: Pairwise Interactions Algorithm
The pairwise interactions algorithm involves T tasks connected in a ring, computing interactions involving N data over T-1 phases. Below are C and Fortran versions of an MPI implementation.

#### C Version
```c
#include "mpi.h"  /* Include file */

main(int argc, char *argv[]) {  /* Main program */
    int myid, np, ierr, lnbr, rnbr;
    real x[300], buff[300], forces[300];
    MPI_Status status;

    ierr = MPI_Init(&argc, &argv);  /* Initialize */
    if(ierr != MPI_SUCCESS) {  /* Check return code */
        // Error handling
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);  /* Get my ID */
    MPI_Comm_size(MPI_COMM_WORLD, &np);  /* Get total processes */

    // Compute left and right neighbors in the ring
    lnbr = (myid - 1 + np) % np;
    rnbr = (myid + 1) % np;

    // Main computation loop
    for (int phase = 0; phase < np - 1; ++phase) {
        MPI_Sendrecv_replace(buff, 300, MPI_REAL, rnbr, 0, lnbr, 0, MPI_COMM_WORLD, &status);
        // Compute interactions here
    }

    MPI_Finalize();  /* Finalize MPI */
}
```

#### Fortran Version
```fortran
PROGRAM main
    INCLUDE 'mpif.h'  ! Include file
    INTEGER :: myid, np, ierr, lnbr, rnbr
    REAL :: x(300), buff(300), forces(300)
    INTEGER :: status(MPI_STATUS_SIZE)

    CALL MPI_INIT(ierr)  ! Initialize
    IF (ierr .NE. MPI_SUCCESS) THEN
        ! Error handling
    END IF
    CALL MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)  ! Get my ID
    CALL MPI_COMM_SIZE(MPI_COMM_WORLD, np, ierr)  ! Get total processes

    ! Compute left and right neighbors in the ring
    lnbr = MOD(myid - 1 + np, np)
    rnbr = MOD(myid + 1, np)

    ! Main computation loop
    DO phase = 1, np - 1
        CALL MPI_SENDRECV_REPLACE(buff, 300, MPI_REAL, rnbr, 0, lnbr, 0, MPI_COMM_WORLD, status, ierr)
        ! Compute interactions here
    END DO

    CALL MPI_FINALIZE(ierr)  ! Finalize MPI
END PROGRAM main
```

###  Determinism

#### Introduction to Determinism
Message-passing programming models are inherently nondeterministic because the arrival order of messages sent from two processes (A and B) to a third process (C) is not defined. MPI guarantees the order of messages sent from one process to another, but ensuring a computation is deterministic is the programmer's responsibility.

#### Ensuring Determinism
- **Source Specifier**: The `MPI_RECV` function allows specifying a source process or accepting messages from any source (`MPI_ANY_SOURCE`). Specifying a source helps avoid errors due to time-dependent message arrival.
- **Message Tags**: Tags help distinguish between different messages. The sender assigns a tag using the `tag` field in `MPI_SEND`, and the receiver can specify which tag to receive or accept any tag (`MPI_ANY_TAG`).

### Example: Nondeterministic Program

The following example demonstrates a nondeterministic MPI program. It implements the symmetric pairwise interaction algorithm, where messages are communicated halfway around a ring and then returned to the originating process.

#### C Version
```c
main(int *argc, char *argv[]) {
    int rnbr, rdest, myid, np;
    float buff[600];
    MPI_Status status;

    MPI_Init(argc, &argv);
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
}
```

This program may suffer from nondeterminism because it uses `MPI_ANY_SOURCE` and `MPI_ANY_TAG`, which can lead to unexpected message order. To ensure determinism, specific sources and tags should be used where possible.

### MPI Global Operations
Parallel algorithms often require coordinated communication among multiple processes. MPI provides specialized collective communication functions to facilitate these operations efficiently. These functions include synchronization, data distribution, and reduction operations.

#### Key MPI Collective Communication Functions


| **Function**          | **Purpose**                                                            | **Parameters**                                                                                                                                 | **Returns**                          |
|-----------------------|------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------|
| **MPI_BARRIER**       | Synchronizes all processes in a communicator.                          | **IN**: `comm` (communicator handle)                                                                                                          | None                                 |
| **MPI_BCAST**         | Broadcasts data from one root process to all other processes.          | **INOUT**: `inbuf` (address of the input buffer) <br> **IN**: `incnt` (number of elements in input buffer) <br> **IN**: `intype` (datatype of input buffer elements) <br> **IN**: `root` (process ID of root process) <br> **IN**: `comm` (communicator handle)  | None                                 |
| **MPI_GATHER**        | Gathers data from all processes to one root process.                   | **IN**: `inbuf` (address of input buffer) <br> **IN**: `incnt` (number of elements sent to each process) <br> **IN**: `intype` (datatype of input buffer elements) <br> **IN**: `outcnt` (number of elements received from each process) <br> **IN**: `outtype` (datatype of output buffer elements) <br> **IN**: `root` (process ID of root process) <br> **IN**: `comm` (communicator handle)  | **OUT**: `outbuf` (address of output buffer) |
| **MPI_SCATTER**       | Distributes data from one root process to all processes.               | **IN**: `inbuf` (address of input buffer) <br> **IN**: `incnt` (number of elements sent to each process) <br> **IN**: `intype` (datatype of input buffer elements) <br> **IN**: `outcnt` (number of elements received from each process) <br> **IN**: `outtype` (datatype of output buffer elements) <br> **IN**: `root` (process ID of root process) <br> **IN**: `comm` (communicator handle)  | **OUT**: `outbuf` (address of output buffer) |
| **MPI_REDUCE**        | Combines values from all processes using a specified operation.        | **IN**: `inbuf` (address of input buffer) <br> **IN**: `count` (number of elements in input buffer) <br> **IN**: `type` (datatype of input buffer elements) <br> **IN**: `op` (reduction operation) <br> **IN**: `root` (process ID of root process) <br> **IN**: `comm` (communicator handle)  | **OUT**: `outbuf` (address of output buffer) |
| **MPI_ALLREDUCE**     | Similar to MPI_REDUCE but returns the result to all processes.         | **IN**: `inbuf` (address of input buffer) <br> **IN**: `count` (number of elements in input buffer) <br> **IN**: `type` (datatype of input buffer elements) <br> **IN**: `op` (reduction operation) <br> **IN**: `comm` (communicator handle)  | **OUT**: `outbuf` (address of output buffer) |

#### Complete Example Program

```c
main(int argc, char *argv[]) {
    MPI_Comm com = MPI_COMM_WORLD;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(com, &np);
    MPI_Comm_rank(com, &me);
    
    if (me == 0) {  /* Read problem size at process 0 */
        read_problem_size(&size);
        buff[0] = size;
    }
    /* Global broadcast propagates this data to all processes */
    MPI_Bcast(buff, 1, MPI_INT, 0, com);
    
    /* Extract problem size from buff; allocate space for local data */
    lsize = buff[0] / np;
    local = malloc(lsize + 2);
    
    /* Read input data at process 0; then distribute to processes */
    if (me == 0) { 
        work = malloc(size); 
        read_array(work); 
    }
    MPI_Scatter(work, lsize, MPI_FLOAT, local + 1, lsize, MPI_FLOAT, 0, com);
    
    lnbr = (me + np - 1) % np;  /* Determine my neighbors in ring */
    rnbr = (me + 1) % np;
    globalerr = 99999.0;
    
    while (globalerr > 0.1) {  /* Repeat until termination */
        /* Exchange boundary values with neighbors */
        ls = local + lsize;
        MPI_Send(local + 1, 1, MPI_FLOAT, lnbr, 10, com);
        MPI_Recv(local + 1, 1, MPI_FLOAT, rnbr, 10, com, &status);
        MPI_Send(ls - 2, 1, MPI_FLOAT, rnbr, 20, com);
        MPI_Recv(ls - 1, 1, MPI_FLOAT, lnbr, 20, com, &status);
        compute(local);
        localerr = maxerror(local);  /* Determine local error */
        
        /* Find maximum local error, and replicate in each process */
        MPI_Allreduce(&localerr, &globalerr, 1, MPI_FLOAT, MPI_MAX, com);
    }
    
    /* Collect results at process 0 */
    MPI_Gather(local, lsize, MPI_FLOAT, work, lsize, MPI_FLOAT, 0, com);
    if (me == 0) { 
        write_array(work); 
        free(work); 
    }
    MPI_Finalize();
}
```

### Finite Difference Problem: Global Operations

The following example demonstrates a finite difference problem using global operations. The algorithm requires both nearest-neighbor communication and global communication to detect termination. This example uses MPI_SEND and MPI_RECV for nearest-neighbor communication and four MPI global communication routines.

#### Communication Operations

1. **MPI_BCAST**: Broadcasts the problem size parameter (`size`) from process 0 to all processes.
2. **MPI_SCATTER**: Distributes an input array (`work`) from process 0 to other processes, so each process receives `size/np` elements.
3. **MPI_SEND and MPI_RECV**: Exchanges data (a single floating-point number) with neighbors.
4. **MPI_ALLREDUCE**: Determines the maximum of a set of `localerr` values computed at different processes and distributes this maximum value to each process.
5. **MPI_GATHER**: Accumulates an output array at process 0.

#### MPI Implementation

The following MPI implementation illustrates the use of these communication operations:

```c
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void read_problem_size(int *size) {
    // Function to read problem size
}

void read_array(float *array) {
    // Function to read input data
}

void write_array(float *array) {
    // Function to write output data
}

void compute(float *local) {
    // Function to perform computation on local data
}

float maxerror(float *local) {
    // Function to compute maximum local error
    return 0.0;
}

int main(int argc, char *argv[]) {
    MPI_Comm com = MPI_COMM_WORLD;
    MPI_Init(&argc, &argv);
    int np, me, size, lsize;
    float buff[1], *work = NULL, *local;
    MPI_Status status;
    int lnbr, rnbr;
    float globalerr, localerr;

    MPI_Comm_size(com, &np);
    MPI_Comm_rank(com, &me);
    
    if (me == 0) {
        read_problem_size(&size);
        buff[0] = size;
    }
    
    MPI_Bcast(buff, 1, MPI_INT, 0, com);
    size = buff[0];
    lsize = size / np;
    local = (float *)malloc((lsize + 2) * sizeof(float));
    
    if (me == 0) {
        work = (float *)malloc(size * sizeof(float));
        read_array(work);
    }
    
    MPI_Scatter(work, lsize, MPI_FLOAT, local + 1, lsize, MPI_FLOAT, 0, com);
    
    lnbr = (me + np - 1) % np;
    rnbr = (me + 1) % np;
    globalerr = 99999.0;
    
    while (globalerr > 0.1) {
        MPI_Send(local + 1, 1, MPI_FLOAT, lnbr, 10, com);
        MPI_Recv(local, 1, MPI_FLOAT, lnbr, 10, com, &status);
        MPI_Send(local + lsize, 1, MPI_FLOAT, rnbr, 20, com);
        MPI_Recv(local + lsize + 1, 1, MPI_FLOAT, rnbr, 20, com, &status);
        
        compute(local);
        localerr = maxerror(local);
        
        MPI_Allreduce(&localerr, &globalerr, 1, MPI_FLOAT, MPI_MAX, com);
    }
    
    MPI_Gather(local + 1, lsize, MPI_FLOAT, work, lsize, MPI_FLOAT, 0, com);
    
    if (me == 0) {
        write_array(work);
        free(work);
    }
    
    free(local);
    MPI_Finalize();
    return 0;
}
```

### MPI Modularity

In parallel programming, modular construction involves combining program components in various ways: sequentially, in parallel, or concurrently. MPI supports modular programming via its communicator mechanism, allowing for the specification of program components that encapsulate internal communication operations and provide a local name space for processes.

#### Communicators in MPI
Communicators in MPI specify the process group and context for communication operations, enabling modular program design. Here are key functions for creating and managing communicators:


| **Function**            | **Purpose**                                                | **Parameters**                                                                                                   | **Returns**                             |
|-------------------------|------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------|-----------------------------------------|
| **MPI_COMM_DUP**        | Creates a new communicator with the same process group but a new context. | **IN**: `comm` (communicator handle)                                                                              | **OUT**: `newcomm` (new communicator handle) |
| **MPI_COMM_SPLIT**      | Partitions a group into disjoint subgroups.                 | **IN**: `comm` (communicator handle) <br> **IN**: `color` (subgroup control, integer) <br> **IN**: `key` (process ID control, integer) | **OUT**: `newcomm` (new communicator handle) |
| **MPI_INTERCOMM_CREATE**| Creates an intercommunicator linking processes in two groups. | **IN**: `comm` (local intracommunicator handle) <br> **IN**: `leader` (local leader, integer) <br> **IN**: `peer` (peer intracommunicator handle) <br> **IN**: `rleader` (process ID of remote leader in `peer`, integer) <br> **IN**: `tag` (tag for communicator setup, integer) | **OUT**: `inter` (new intercommunicator handle) |
| **MPI_COMM_FREE**       | Destroys a communicator.                                    | **IN**: `comm` (communicator handle)                                                                             | None                                    |

#### Creating Communicators

##### Using MPI_COMM_DUP for Sequential Composition
Sequential composition can cause issues if different components use the same message tags. Using different contexts via MPI_COMM_DUP avoids this problem.

Example:
```fortran
integer comm, newcomm, ierr
call MPI_COMM_DUP(comm, newcomm, ierr)
call transpose(newcomm, A)
call MPI_COMM_FREE(newcomm, ierr)
```

##### Using MPI_COMM_SPLIT for Parallel Composition
MPI_COMM_SPLIT partitions processes into subgroups, allowing parallel execution without conflicts.

Example:
```c
MPI_Comm comm, newcomm;
int myid, color;
MPI_Comm_rank(comm, &myid);
color = myid % 3;
MPI_Comm_split(comm, color, myid, &newcomm);
```
This splits the communicator into subgroups based on `myid % 3`.

### Communicating between Groups
An intercommunicator created by MPI_INTERCOMM_CREATE links two groups, allowing intergroup communication.

Example:
```c
MPI_Comm comm, newcomm, intercomm;
int myid, new_id, np;
MPI_Comm_rank(MPI_COMM_WORLD, &myid);
MPI_Comm_size(MPI_COMM_WORLD, &np);
int color = (myid % 2 == 0) ? 0 : 1;
MPI_Comm_split(MPI_COMM_WORLD, color, myid, &newcomm);
int local_leader = (myid % 2 == 0) ? 0 : 1;
int remote_leader = 1 - local_leader;
MPI_Intercomm_create(newcomm, local_leader, MPI_COMM_WORLD, remote_leader, 0, &intercomm);
```

#### Example Program: Intercommunicator
The following program splits processes into two groups and creates an intercommunicator between them.

```c
#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    MPI_Comm comm = MPI_COMM_WORLD, newcomm, intercomm;
    int myid, np, color, new_id;
    
    MPI_Comm_rank(comm, &myid);
    MPI_Comm_size(comm, &np);
    
    color = (myid % 2 == 0) ? 0 : 1;
    MPI_Comm_split(comm, color, myid, &newcomm);
    
    int local_leader = 0;
    int remote_leader = (color == 0) ? 1 : 0;
    
    MPI_Intercomm_create(newcomm, local_leader, comm, remote_leader, 0, &intercomm);
    
    if (color == 0) {
        MPI_Send(&myid, 1, MPI_INT, 0, 0, intercomm);
    } else {
        int received;
        MPI_Recv(&received, 1, MPI_INT, 0, 0, intercomm, MPI_STATUS_IGNORE);
        printf("Process %d received message from process %d\n", myid, received);
    }
    
    MPI_Comm_free(&newcomm);
    MPI_Comm_free(&intercomm);
    MPI_Finalize();
    return 0;
}
```

### MPI Derived Datatypes
In MPI, derived datatypes allow grouping noncontiguous data elements into a single message, avoiding the need for data copy operations. This mechanism is crucial for efficiently communicating complex data structures, such as rows of a 2D array stored by columns.

#### Key Functions for Derived Datatypes

| **Function**            | **Purpose**                                                         | **Parameters**                                                                                                 | **Returns**                             |
|-------------------------|---------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------|-----------------------------------------|
| **MPI_TYPE_CONTIGUOUS** | Creates a derived datatype from contiguous elements.                | **IN**: `count` (number of elements, integer ≥ 0) <br> **IN**: `oldtype` (input datatype handle)               | **OUT**: `newtype` (output datatype handle) |
| **MPI_TYPE_VECTOR**     | Creates a derived datatype from blocks separated by a stride.       | **IN**: `count` (number of elements, integer ≥ 0) <br> **IN**: `blocklen` (elements in a block, integer ≥ 0) <br> **IN**: `stride` (elements between start of each block, integer) <br> **IN**: `oldtype` (input datatype handle) | **OUT**: `newtype` (output datatype handle) |
| **MPI_TYPE_INDEXED**    | Creates a derived datatype with variable indices and sizes.         | **IN**: `count` (number of blocks, integer ≥ 0) <br> **IN**: `blocklens` (elements in each block, array of integer ≥ 0) <br> **IN**: `indices` (displacements for each block, array of integer) <br> **IN**: `oldtype` (input datatype handle) | **OUT**: `newtype` (output datatype handle) |
| **MPI_TYPE_COMMIT**     | Commits a datatype so that it can be used in communication.         | **INOUT**: `type` (datatype to be committed handle)                                                            | None                                    |
| **MPI_TYPE_FREE**       | Frees a derived datatype.                                           | **INOUT**: `type` (datatype to be freed handle)                                                                | None                                    |

#### Example Program:  Contiguous Derived Type

Example of defining a contiguous derived type:
```fortran
integer ierr
call MPI_TYPE_CONTIGUOUS(10, MPI_REAL, tenrealtype, ierr)
call MPI_TYPE_COMMIT(tenrealtype, ierr)
call MPI_SEND(data, 1, tenrealtype, dest, tag, MPI_COMM_WORLD, ierr)
call MPI_TYPE_FREE(tenrealtype, ierr)
```
This is equivalent to:
```fortran
call MPI_SEND(data, 10, MPI_REAL, dest, tag, MPI_COMM_WORLD, ierr)
```

#### Example Program:  Vector Derived Type

Example of defining a vector derived type:

```c
float data[1024];
MPI_Datatype floattype;
MPI_Type_vector(10, 1, 32, MPI_FLOAT, &floattype);
MPI_Type_commit(&floattype);
MPI_Send(data, 1, floattype, dest, tag, MPI_COMM_WORLD);
MPI_Type_free(&floattype);
```

This is equivalent to:

```c
float data[1024], buff[10];
for (int i = 0; i < 10; i++) buff[i] = data[i * 32];
MPI_Send(buff, 10, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
```

#### Example Program: Finite Difference Stencil

This program uses derived types to communicate the north and south rows and the west and east columns of a Fortran array.

```fortran
integer, parameter :: n = 10, m = 10
real :: array(n, m)
integer :: ierr, rowtype, coltype

! Define row type
call MPI_TYPE_VECTOR(m, 1, n, MPI_REAL, rowtype, ierr)
call MPI_TYPE_COMMIT(rowtype, ierr)

! Define column type
call MPI_TYPE_CONTIGUOUS(n, MPI_REAL, coltype, ierr)
call MPI_TYPE_COMMIT(coltype, ierr)

! Use derived types to send/receive rows and columns
call MPI_SEND(array(1, 1), 1, rowtype, dest, tag, MPI_COMM_WORLD, ierr)
call MPI_RECV(array(1, 1), 1, coltype, source, tag, MPI_COMM_WORLD, ierr)

! Free derived types
call MPI_TYPE_FREE(rowtype, ierr)
call MPI_TYPE_FREE(coltype, ierr)
```

#### Example Program: Fock Matrix Problem

This example uses MPI_TYPE_INDEXED to send noncontiguous blocks of data.

```fortran
integer, parameter :: len = 100
integer :: focktype, ierr, inbuf(len), source

! Define indexed type
call MPI_TYPE_INDEXED(len / 2, inbuf(len / 2 + 1), inbuf(1), MPI_INTEGER, focktype, ierr)
call MPI_TYPE_COMMIT(focktype, ierr)

! Use indexed type to send data
call MPI_SEND(data, 1, focktype, source, MPI_COMM_WORLD, ierr)

! Free indexed type
call MPI_TYPE_FREE(focktype, ierr)
```

### Asynchronous Communication

Asynchronous communication is essential when a computation must access elements of a shared data structure in an unstructured manner. MPI provides functions like MPI_IPROBE, MPI_PROBE, and MPI_GET_COUNT to facilitate asynchronous communication.

| **Function**          | **Purpose**                                                            | **Parameters**                                                                                                                    | **Returns**                                |
|-----------------------|------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------|
| **MPI_IPROBE**        | Polls for a pending message without receiving it.                      | **IN**: `source` (ID of source process, or `MPI_ANY_SOURCE`) <br> **IN**: `tag` (Message tag, or `MPI_ANY_TAG`) <br> **IN**: `comm` (communicator handle) | **OUT**: `flag` (Boolean indicating if a message is available) <br> **OUT**: `status` (Status object) |
| **MPI_PROBE**         | Blocks until a message is pending, then returns and sets its status argument. | **IN**: `source` (ID of source process, or `MPI_ANY_SOURCE`) <br> **IN**: `tag` (Message tag, or `MPI_ANY_TAG`) <br> **IN**: `comm` (communicator handle) | **OUT**: `status` (Status object) |
| **MPI_GET_COUNT**     | Determines the size of a message.                                      | **IN**: `status` (Status variable from receive) <br> **IN**: `datatype` (Datatype of receive buffer elements)                       | **OUT**: `count` (Number of data elements in message) |

#### Example Program: Using Asynchronous Communication

The following code demonstrates how to use MPI_IPROBE, MPI_PROBE, and MPI_GET_COUNT to handle messages from an unknown source containing an unknown number of integers.

```c
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    MPI_Status status;
    int flag, count, *buf, source;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(&argc, &argv);

    MPI_Probe(MPI_ANY_SOURCE, 0, comm, &status);
    source = status.MPI_SOURCE;
    MPI_Get_count(&status, MPI_INT, &count);
    buf = (int *)malloc(count * sizeof(int));
    MPI_Recv(buf, count, MPI_INT, source, 0, comm, &status);

    // Process the received message
    free(buf);
    MPI_Finalize();
    return 0;
}
```

#### Example Program: Fock Matrix Construction with Polling

This example integrates data and computation tasks into a single process, alternating between checking for pending data requests and performing computation.

```c
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void process_request(MPI_Status status) {
    int address;
    float value;
    int source = status.MPI_SOURCE;
    MPI_Recv(&address, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
    value = data[address]; // Assuming data is defined and populated elsewhere
    MPI_Send(&value, 1, MPI_FLOAT, source, 5, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
    MPI_Status status;
    int flag, done = FALSE;

    MPI_Init(&argc, &argv);

    while (done != TRUE) {
        MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
        while (flag == TRUE) {
            process_request(status);
            MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);
        }
        // Perform other tasks
        identify_next_task();
        generate_requests();
        process_replies();
    }

    MPI_Finalize();
    return 0;
}
```

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
