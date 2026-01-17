# Multiprocessing Quiz

Test your understanding of processes, inter-process communication, and process management.

---

#### Q. How should the average rates of production and consumption be related in a producer-consumer architecture?

- [ ] The consumption and production rates must be exactly the same.
- [ ] The consumption rate should be greater than or equal to the production rate.
- [ ] It does not matter.
- [x] The consumption rate should be less than or equal to the production rate.
- [ ] The production rate should always exceed the consumption rate.

#### Q. Which architecture consists of a chained together series of producer-consumer pairs?

- [ ] FIFO
- [x] pipeline
- [ ] distributed
- [ ] client-server
- [ ] hierarchical

#### Q. In most operating systems the _____ determines when each of the threads and processes gets scheduled to execute.

- [ ] operating system
- [x] processor hardware
- [ ] application software
- [ ] user
- [ ] scheduler algorithm

#### Q. What is a key advantage of using processes over threads?

- [ ] Processes share memory by default
- [ ] Processes are faster to create
- [x] Processes provide better isolation and fault tolerance
- [ ] Processes can run on a single core only
- [ ] Processes don't require synchronization

#### Q. In Python's multiprocessing module, which method is used to start a new process?

- [ ] run()
- [ ] execute()
- [x] start()
- [ ] begin()
- [ ] launch()

#### Q. What is the purpose of a Queue in multiprocessing?

- [ ] To organize processes by priority
- [ ] To limit the number of processes
- [x] To enable safe inter-process communication
- [ ] To store process IDs
- [ ] To synchronize process clocks

#### Q. In Python, which class from the multiprocessing module is used to share state between processes?

- [ ] SharedState
- [ ] ProcessState
- [x] Value or Array
- [ ] SharedMemory only
- [ ] GlobalVar

#### Q. What is the difference between fork() and spawn() process creation methods?

- [ ] fork() is faster; spawn() is slower
- [x] fork() copies the parent process; spawn() starts a fresh Python interpreter
- [ ] fork() is for Windows; spawn() is for Linux
- [ ] There is no difference
- [ ] fork() is deprecated

#### Q. In Python's multiprocessing, what is the purpose of Pool?

- [ ] To limit memory usage
- [ ] To create shared variables
- [x] To manage a pool of worker processes for parallel task execution
- [ ] To store process results
- [ ] To handle process exceptions

#### Q. What is inter-process communication (IPC)?

- [ ] Communication between threads in the same process
- [x] Mechanisms that allow processes to exchange data and signals
- [ ] Communication between CPUs
- [ ] Network communication between computers
- [ ] Communication between the OS and processes

#### Q. Which of the following is NOT a common IPC mechanism?

- [ ] Pipes
- [ ] Shared memory
- [ ] Message queues
- [x] Thread-local storage
- [ ] Sockets

#### Q. What is a "zombie process"?

- [ ] A process that has crashed
- [ ] A process that runs slowly
- [x] A process that has completed execution but still has an entry in the process table
- [ ] A process that consumes too much memory
- [ ] A process that cannot be terminated

#### Q. In Python's multiprocessing, what happens if a child process raises an exception?

- [ ] The parent process crashes
- [ ] The exception is silently ignored
- [x] The exception is propagated when join() or get() is called
- [ ] All child processes are terminated
- [ ] The exception is logged automatically

#### Q. What is the purpose of the terminate() method in Python's multiprocessing module?

- [ ] To gracefully shut down a process
- [x] To forcefully stop a process immediately
- [ ] To pause a process
- [ ] To restart a process
- [ ] To send a signal to a process

#### Q. What is a "pipe" in the context of multiprocessing?

- [ ] A scheduling algorithm
- [x] A one-way or two-way communication channel between processes
- [ ] A memory allocation technique
- [ ] A type of process pool
- [ ] A debugging tool

#### Q. In Python's multiprocessing, what is the difference between map() and apply_async()?

- [ ] map() is for single tasks; apply_async() is for multiple tasks
- [x] map() blocks until all results are ready; apply_async() returns immediately
- [ ] map() is faster; apply_async() is slower
- [ ] map() is deprecated; apply_async() is the new method
- [ ] There is no difference

#### Q. What is a "process group"?

- [ ] A pool of worker processes
- [x] A collection of related processes that can receive signals together
- [ ] Processes sharing the same memory
- [ ] Processes on the same CPU core
- [ ] A debugging concept only

#### Q. Which statement about orphan processes is correct?

- [ ] Orphan processes are automatically terminated
- [ ] Orphan processes cannot run
- [x] Orphan processes are adopted by the init (PID 1) process
- [ ] Orphan processes cause memory leaks
- [ ] Orphan processes are the same as zombie processes

#### Q. In Python's multiprocessing, what does the daemon attribute do when set to True?

- [ ] Makes the process run with elevated privileges
- [x] Makes the process terminate when the main program exits
- [ ] Makes the process run in the background
- [ ] Makes the process immune to signals
- [ ] Makes the process share memory with other daemons

#### Q. What is the Global Interpreter Lock (GIL) and how does multiprocessing relate to it?

- [ ] GIL prevents all parallelism in Python
- [ ] Multiprocessing also uses the GIL
- [x] Multiprocessing bypasses the GIL by using separate processes with their own interpreter
- [ ] GIL only affects multiprocessing, not threading
- [ ] GIL was removed in Python 3

#### Q. What is "copy-on-write" in the context of process forking?

- [ ] A technique to copy processes faster
- [x] An optimization where memory is shared until either process writes to it
- [ ] A way to write to multiple processes simultaneously
- [ ] A file copying technique for processes
- [ ] A debugging feature

#### Q. In Python, which is more appropriate for CPU-bound tasks: threading or multiprocessing?

- [ ] Threading, because it has less overhead
- [x] Multiprocessing, because it can use multiple CPU cores effectively
- [ ] Neither, Python cannot handle CPU-bound tasks
- [ ] Both perform equally well
- [ ] Threading, because the GIL helps with CPU tasks

#### Q. What is the purpose of Manager in Python's multiprocessing module?

- [ ] To manage process priorities
- [ ] To monitor process health
- [x] To create shared objects that can be modified by multiple processes
- [ ] To handle exceptions in processes
- [ ] To manage memory allocation

#### Q. What happens to file descriptors when a process is forked?

- [ ] They are closed in the child process
- [x] They are duplicated in the child process
- [ ] They become invalid
- [ ] They are shared with read-only access
- [ ] They point to different files

#### Q. What is process affinity?

- [ ] A measure of how well processes work together
- [x] The ability to bind a process to specific CPU cores
- [ ] The memory sharing between processes
- [ ] The priority level of a process
- [ ] The relationship between parent and child processes
