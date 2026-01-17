# Distributed Computing and MPI Quiz

Test your understanding of distributed systems, message passing, and MPI concepts.

---

#### Q. Distributed memory architectures are often considered _____ compared to shared memory systems.

- [ ] more responsive
- [x] more scalable
- [ ] easier to program for
- [ ] less complex
- [ ] more cost-effective

#### Q. Which scenario describes the best use case for a point-to-point communication strategy?

- [ ] A small number of tasks need to send data a large number of other tasks.
- [ ] A large number of tasks will be sending data to a small number of receiving tasks.
- [ ] A large number of tasks need to communicate with each other.
- [x] A small number of tasks need to communicate with each other.
- [ ] All tasks need to broadcast to all other tasks.

#### Q. In MPI, what is a "rank"?

- [ ] The priority of a process
- [x] A unique identifier for each process in a communicator
- [ ] The speed of a process
- [ ] The memory size allocated to a process
- [ ] The CPU core assignment

#### Q. What is the purpose of MPI_COMM_WORLD?

- [ ] To communicate with external systems
- [x] The default communicator that includes all processes in an MPI program
- [ ] To create new processes
- [ ] To handle global variables
- [ ] To manage file I/O

#### Q. What is the difference between blocking and non-blocking MPI communication?

- [ ] Blocking is faster
- [ ] Non-blocking is more reliable
- [x] Blocking waits until the operation completes; non-blocking returns immediately and operation continues in background
- [ ] Blocking is for large messages only
- [ ] There is no significant difference

#### Q. What is MPI_Barrier() used for?

- [ ] To block messages from other processes
- [ ] To limit communication bandwidth
- [x] To synchronize all processes in a communicator; no process proceeds until all have reached the barrier
- [ ] To create a new communicator
- [ ] To handle errors

#### Q. What is collective communication in MPI?

- [ ] Communication between two processes
- [x] Communication operations that involve all processes in a communicator
- [ ] Communication over the network
- [ ] Communication using shared memory
- [ ] Communication between different MPI programs

#### Q. What does MPI_Bcast() do?

- [ ] Broadcasts a message to all computers in a network
- [x] Sends data from one process (root) to all other processes in the communicator
- [ ] Broadcasts an error message
- [ ] Creates new processes
- [ ] Collects data from all processes

#### Q. What is MPI_Reduce() used for?

- [ ] To reduce the number of processes
- [ ] To compress message size
- [x] To combine data from all processes using an operation (like sum) and store result at root process
- [ ] To reduce memory usage
- [ ] To reduce communication overhead

#### Q. What is the difference between MPI_Scatter and MPI_Gather?

- [ ] Scatter is faster than Gather
- [x] Scatter distributes data from root to all processes; Gather collects data from all processes to root
- [ ] They do the same thing
- [ ] Scatter is for small data; Gather is for large data
- [ ] Scatter is synchronous; Gather is asynchronous

#### Q. In MPI, what is a communicator?

- [ ] A process that handles communication
- [x] A group of processes that can communicate with each other
- [ ] A network connection
- [ ] A message buffer
- [ ] A synchronization primitive

#### Q. What is MPI_Allreduce()?

- [ ] A reduce operation that fails silently
- [x] A reduce operation where all processes get the result, not just the root
- [ ] A faster version of MPI_Reduce
- [ ] A reduce that works on all data types
- [ ] An asynchronous reduce operation

#### Q. What is "load balancing" in distributed computing?

- [ ] Balancing memory usage
- [ ] Balancing network traffic
- [x] Distributing work evenly across processors to maximize efficiency
- [ ] Balancing power consumption
- [ ] Balancing data storage

#### Q. What is the difference between strong and weak scaling?

- [ ] Strong is for large problems; weak is for small problems
- [x] Strong scaling: fixed problem size with more processors; weak scaling: problem size grows with processors
- [ ] They are the same concept
- [ ] Strong is theoretical; weak is practical
- [ ] Strong uses more memory

#### Q. What is a "ghost cell" or "halo exchange" in parallel computing?

- [ ] A cell that stores deleted data
- [x] Boundary data copied from neighboring processes to enable local computation
- [ ] A debugging technique
- [ ] A cell that is not used
- [ ] An error handling mechanism

#### Q. What is MPI_Sendrecv() used for?

- [ ] To send and receive to the same process
- [x] To simultaneously send to one process and receive from another, avoiding potential deadlock
- [ ] To send large messages
- [ ] To send messages reliably
- [ ] To send encrypted messages

#### Q. What is the purpose of MPI_Finalize()?

- [ ] To send final messages
- [ ] To wait for all messages to be delivered
- [x] To clean up MPI resources and terminate MPI operations
- [ ] To finalize computation results
- [ ] To synchronize all processes one last time

#### Q. What is "domain decomposition" in parallel computing?

- [ ] Breaking down the problem domain into smaller domains
- [x] Dividing a computational domain among multiple processors, each handling a subdomain
- [ ] Decomposing complex functions into simpler ones
- [ ] Separating data from code
- [ ] Breaking up network domains

#### Q. What is the difference between MPI_Send and MPI_Isend?

- [ ] MPI_Send is safer
- [x] MPI_Send blocks until buffer can be reused; MPI_Isend returns immediately (non-blocking)
- [ ] MPI_Isend is for integers only
- [ ] MPI_Send is deprecated
- [ ] There is no difference

#### Q. What is a "process topology" in MPI?

- [ ] The physical arrangement of computers
- [x] A virtual arrangement of processes that reflects the communication pattern of an application
- [ ] The network topology
- [ ] The memory layout
- [ ] The scheduling order

#### Q. What is MPI_Comm_size() used for?

- [ ] To get the size of a message
- [ ] To get memory size
- [x] To get the total number of processes in a communicator
- [ ] To get the buffer size
- [ ] To get the network bandwidth

#### Q. What is "latency hiding" in distributed computing?

- [ ] Hiding network delays from users
- [x] Overlapping communication with computation to reduce the impact of communication delays
- [ ] Reducing latency through compression
- [ ] A debugging technique
- [ ] Caching frequently used data

#### Q. What is the purpose of MPI_Wait()?

- [ ] To make a process wait for user input
- [x] To wait for a non-blocking operation to complete
- [ ] To pause execution
- [ ] To synchronize clocks
- [ ] To wait for a barrier

#### Q. What is a "collective" in MPI terminology?

- [ ] A group of messages
- [x] An operation that must be called by all processes in a communicator
- [ ] A shared variable
- [ ] A type of buffer
- [ ] A process group

#### Q. What is MPI_Alltoall() used for?

- [ ] To broadcast to all processes
- [x] To send distinct data from each process to every other process (complete exchange)
- [ ] To reduce data from all processes
- [ ] To synchronize all processes
- [ ] To gather data at all processes

#### Q. What is "message tagging" in MPI?

- [ ] Adding labels for debugging
- [x] Integer values used to distinguish different types of messages between the same pair of processes
- [ ] Compressing message headers
- [ ] Logging message traffic
- [ ] Encrypting messages
