# Basic Terminology Quiz

Test your understanding of fundamental concepts in parallel and concurrent programming.

---

#### Q. A _____ contains one or more _____.

- [ ] process; other processes
- [ ] thread; processes
- [ ] thread; other threads
- [x] process; threads

#### Q. Parallel computing primarily enhances the _____.

- [ ] number of tasks a program executes in a set time
- [x] speed at which a program executes a set number of tasks
- [ ] scale of problems a program can tackle
- [ ] all of these answers
- [ ] efficiency of memory usage

#### Q. It is possible for two tasks to execute _____ using a single-core processor.

- [ ] in parallel
- [x] concurrently
- [ ] concurrently or in parallel
- [ ] sequentially

#### Q. You can safely expect threads to execute in the same relative order that you create them.

- [ ] TRUE
- [x] FALSE
- [ ] It depends on the operating system
- [ ] It depends on the processor

#### Q. Flynn's Taxonomy categorizes computer architectures based on the number of concurrent _____ streams and _____ streams.

- [ ] input; output
- [x] instruction; data
- [ ] program; data
- [ ] memory; I/O
- [ ] data; cache

#### Q. Which classification of Flynn's Taxonomy do modern multi-core PCs fall under?

- [ ] SIMD
- [ ] MISD
- [ ] SISD
- [x] MIMD
- [ ] SPMD

#### Q. Every thread is independent and has its own separate address space in memory.

- [x] FALSE
- [ ] TRUE
- [ ] It depends on the operating system
- [ ] It depends on the language used

#### Q. Processes _____ than threads.

- [ ] are simpler to communicate between
- [ ] are considered more "lightweight"
- [ ] are faster to switch between
- [x] require more overhead to create

#### Q. The operating system assigns each process a unique _____.

- [x] process ID number
- [ ] CPU core
- [ ] process name
- [ ] number of threads
- [ ] memory address

#### Q. A race condition _____ a data race.

- [ ] only occurs when there is
- [x] can occur independently of
- [ ] is just another name for
- [ ] is more severe than
- [ ] is unrelated to

#### Q. Which scenario creates the potential for a race condition to occur?

- [ ] the order in which two threads execute their respective operations will change the output
- [x] one thread is modifying a shared variable while another thread concurrently reads its value
- [ ] a single-threaded program is competing with other processes for execution time on the CPU
- [ ] two threads are concurrently reading and writing the same shared variable
- [ ] multiple threads are waiting for a condition variable to be notified

#### Q. Which of these is responsible for causing a race condition?

- [ ] corrupted memory
- [ ] the execution scheduler
- [ ] a slow Internet connection
- [x] not using a semaphore
- [ ] inadequate CPU resources

#### Q. What is the difference between concurrency and parallelism?

- [x] Concurrency is about managing multiple tasks at once; parallelism is about executing multiple tasks simultaneously
- [ ] Concurrency requires multiple processors; parallelism can work on a single processor
- [ ] They are the same concept with different names
- [ ] Parallelism is about I/O operations; concurrency is about CPU operations
- [ ] Concurrency is faster than parallelism

#### Q. What is the primary purpose of synchronization in concurrent programming?

- [ ] To speed up program execution
- [x] To coordinate access to shared resources and prevent data corruption
- [ ] To reduce memory usage
- [ ] To simplify program logic
- [ ] To improve cache performance

#### Q. In the context of concurrent programming, what is "mutual exclusion"?

- [ ] A mechanism to prevent program termination
- [ ] A way to share data between processes
- [x] Ensuring only one thread can access a critical section at a time
- [ ] A method to parallelize sequential code
- [ ] A technique for load balancing

#### Q. What is a "critical section" in concurrent programming?

- [ ] The main function of a program
- [ ] A section of code that runs very fast
- [x] A section of code that accesses shared resources and must not be executed by multiple threads simultaneously
- [ ] The initialization code of a program
- [ ] A section that handles errors

#### Q. What does "thread-safe" mean?

- [ ] A thread that cannot crash
- [ ] A thread that runs in a protected environment
- [x] Code that can be safely executed by multiple threads concurrently without data corruption
- [ ] A thread that has priority over others
- [ ] A thread that is immune to deadlocks

#### Q. What is a "deadlock" in concurrent programming?

- [ ] A program that stops responding
- [x] A situation where two or more threads are waiting for each other to release resources, causing all to be blocked indefinitely
- [ ] A memory leak in threaded applications
- [ ] A situation where a thread runs too fast
- [ ] A crashed program

#### Q. Which of the following best describes "starvation" in concurrent programming?

- [ ] A thread that consumes too much memory
- [ ] A thread that runs without stopping
- [x] A situation where a thread is perpetually denied access to resources it needs
- [ ] A deadlock between multiple threads
- [ ] A thread that has no work to do

#### Q. What is a "context switch"?

- [ ] Switching between different programming languages
- [x] The process of saving and restoring the state of a thread/process so another can run
- [ ] Switching between user mode and kernel mode
- [ ] Changing the priority of a thread
- [ ] Switching between different memory regions

#### Q. What is the difference between preemptive and cooperative multitasking?

- [x] In preemptive, the OS can interrupt tasks; in cooperative, tasks must voluntarily yield
- [ ] Preemptive is faster than cooperative
- [ ] Cooperative requires more memory
- [ ] They are the same thing with different names
- [ ] Preemptive is only for single-core systems

#### Q. What is "atomic operation" in concurrent programming?

- [ ] An operation that uses atomic energy
- [ ] A very small operation
- [x] An operation that completes in a single step without interruption
- [ ] An operation that cannot fail
- [ ] An operation that is very fast

#### Q. What is "priority inversion"?

- [ ] When low priority tasks run before high priority ones intentionally
- [x] When a high-priority task waits for a low-priority task holding a needed resource
- [ ] When all tasks have the same priority
- [ ] When priority values are inverted in memory
- [ ] A debugging technique

#### Q. What is the difference between a "heavyweight" and "lightweight" process?

- [ ] Heavyweight processes use more memory
- [ ] Lightweight processes are faster
- [x] Threads are lightweight (share address space); processes are heavyweight (separate address space)
- [ ] Heavyweight processes have higher priority
- [ ] There is no difference

#### Q. What is "deterministic" execution in concurrent programming?

- [ ] Execution that is predetermined by the OS
- [x] Execution that produces the same result given the same inputs, regardless of scheduling
- [ ] Execution that uses a fixed amount of memory
- [ ] Execution that runs in constant time
- [ ] Execution that cannot be interrupted

#### Q. What is a "memory model" in concurrent programming?

- [ ] A model of physical RAM
- [x] Rules defining how memory operations appear to execute relative to each other across threads
- [ ] A diagram of memory layout
- [ ] A technique for memory allocation
- [ ] A debugging tool for memory leaks

#### Q. What causes "non-deterministic" behavior in concurrent programs?

- [ ] Bugs in the code
- [ ] Using too many threads
- [x] Different possible interleavings of thread execution depending on scheduling
- [ ] Using different compilers
- [ ] Running on different hardware

#### Q. What is the "happens-before" relationship?

- [ ] A debugging concept
- [x] A guarantee that one operation's effects are visible to another operation
- [ ] The order in which code is written
- [ ] The order in which functions are called
- [ ] A compilation order constraint
