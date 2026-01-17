# Performance Evaluation Quiz

Test your understanding of performance metrics, speedup, efficiency, and scaling in parallel computing.

---

#### Q. Which of these describes a program's "latency"?

- [ ] ratio of sequential execution time to the parallel execution time with some number of processors
- [x] amount of time a task takes to execute
- [ ] number of tasks that can be executed in a certain amount of time
- [ ] number of processors used by the program

#### Q. If 85% of a program is parallelizable so that using a 6-core processor will produce a 6x speedup for that portion of the code, what is the maximum overall speedup the program can achieve?

- [x] 4.62
- [ ] 5.1
- [ ] 1
- [ ] 6
- [ ] 5.5

#### Q. What does calculating a program's efficiency (speedup divided by number of parallel processors) provide an indicator of?

- [ ] the maximum number of parallel processors the program can utilize
- [ ] how well the parallel program is performing compared to its sequential implementation
- [x] how well the parallel processing resources are being utilized
- [ ] the optimal number of parallel processors for the program to use
- [ ] the minimum number of processors needed

#### Q. What is an advantage of using fine-grained parallelism with a large number of small tasks?

- [ ] high computation-to-communication ratio
- [ ] poor load balancing
- [ ] low computation-to-communication ratio
- [x] good load-balancing
- [ ] reduced synchronization overhead

#### Q. Which of these describes a program's "throughput"?

- [x] number of tasks that can be executed in a certain amount of time
- [ ] ratio of sequential execution time to the parallel execution time with some number of processors
- [ ] amount of time a task takes to execute
- [ ] number of processors used by the program

#### Q. Which of these describes a program's "critical path"?

- [x] longest series of sequential operations through the program
- [ ] sum of the time for all task nodes in a computational graph
- [ ] sum of the time for all task nodes along the critical path
- [ ] shortest execution path through the program

#### Q. What does a work-to-span ratio less than one indicate?

- [ ] A parallelized version of the program will execute faster than the sequential version.
- [ ] A parallelized version of the program will execute in the same amount of time as the sequential version.
- [ ] The work-to-span ratio cannot be less than one.
- [x] A parallelized version of the program will execute slower than the sequential version.
- [ ] The program has no parallelism opportunities.

#### Q. Increasing the number of processors with a fixed problem size per processor leverages weak scaling to accomplish _____ in _____.

- [ ] more work; more time
- [ ] same work; less time
- [x] more work; less time
- [ ] more work; same time
- [ ] less work; same time

#### Q. Which of these describes a program's "work"?

- [ ] sum of the time for all task nodes along the critical path
- [ ] longest series of sequential operations through the program
- [x] sum of the time for all task nodes in a computational graph
- [ ] total number of operations performed by the program

#### Q. Why should you average the execution time across multiple runs when measuring a program's performance?

- [ ] The system clock used to determine the start and end times of the program is inconsistent and may fluctuate randomly.
- [ ] It's good luck to do things multiple times.
- [ ] The program might crash so you should measure it multiple times to make sure you get at least one good run.
- [x] The execution time will vary from run-to-run depending on how the operating system chooses to schedule your program.
- [ ] To account for hardware variability.

#### Q. Which of these describes a program's "span"?

- [ ] longest series of sequential operations through the program
- [x] sum of the time for all task nodes along the critical path
- [ ] sum of the time for all task nodes in a computational graph
- [ ] total execution time of the program

#### Q. Increasing the number of processors with a fixed total problem size leverages strong scaling to accomplish _____ in _____.

- [ ] more work; less time
- [x] same work; less time
- [ ] more work; same time
- [ ] more work; more time
- [ ] less work; more time

#### Q. Amdahl's Law calculates a(n) _____ for the overall speedup that parallelizing a program will achieve.

- [ ] upper and lower limit
- [ ] lower limit
- [x] upper limit
- [ ] average value
- [ ] median value

#### Q. What is an advantage of using coarse-grained parallelism with a small number of large tasks?

- [x] high computation-to-communication ratio
- [ ] poor load balancing
- [ ] low computation-to-communication ratio
- [ ] good load-balancing
- [ ] reduced memory overhead

#### Q. Which of these describes a program's "speedup"?

- [ ] amount of time a task takes to execute
- [x] ratio of sequential execution time to the parallel execution time with some number of processors
- [ ] number of tasks that can be executed in a certain amount of time
- [ ] number of processors used by the program

#### Q. Granularity can be described as the ratio of _____ over _____.

- [ ] speedup; number of processors
- [ ] tasks; time
- [ ] tasks; number of processors
- [x] computation; communication
- [ ] workload; processors

#### Q. Why are computational graphs useful?

- [x] They include all of the tasks within a program, from start to finish.
- [ ] They help to identify critical sections of code that will require mutual exclusion.
- [ ] They provide a good excuse to practice your drawing skills.
- [x] They help to identify opportunities for parallel execution.
- [ ] They simplify the debugging process.

#### Q. What is "parallel overhead"?

- [ ] The memory used by parallel programs
- [x] Extra time spent on coordination, communication, and synchronization that wouldn't exist in sequential execution
- [ ] The cost of parallel hardware
- [ ] The complexity of parallel code
- [ ] The power consumption of parallel systems

#### Q. What is the "scalability" of a parallel program?

- [ ] How large the program can grow
- [x] How well performance improves as more processors are added
- [ ] How many features the program has
- [ ] The maximum number of processors it can use
- [ ] The size of problems it can solve

#### Q. What is Gustafson's Law and how does it differ from Amdahl's Law?

- [ ] They are the same law
- [x] Gustafson's Law considers that problem size can grow with processors (weak scaling), showing more optimistic speedup potential
- [ ] Gustafson's Law is for single processors
- [ ] Gustafson's Law is deprecated
- [ ] Gustafson's Law applies only to shared memory

#### Q. What is "parallel efficiency"?

- [ ] How efficiently parallel code is written
- [x] The ratio of speedup to the number of processors (speedup/P), measuring resource utilization
- [ ] The energy efficiency of parallel computing
- [ ] The memory efficiency of parallel programs
- [ ] The ratio of communication to computation

#### Q. What is "load imbalance" in parallel computing?

- [ ] An error in load balancing algorithms
- [x] When work is unevenly distributed among processors, causing some to wait while others are busy
- [ ] When the load is too heavy for processors
- [ ] A network bandwidth issue
- [ ] A memory allocation problem

#### Q. What is "isoefficiency" in the context of parallel computing?

- [ ] Equal efficiency across all processors
- [x] The rate at which problem size must grow with the number of processors to maintain constant efficiency
- [ ] A measure of energy efficiency
- [ ] The efficiency of I/O operations
- [ ] A debugging metric

#### Q. What is the difference between "wall clock time" and "CPU time"?

- [ ] They are the same thing
- [x] Wall clock time is actual elapsed time; CPU time is only the time the CPU spent on the task
- [ ] Wall clock is for sequential; CPU time is for parallel
- [ ] Wall clock includes sleep time; CPU time doesn't
- [ ] CPU time is always longer

#### Q. What is "communication overhead" in parallel computing?

- [ ] The bandwidth of the network
- [x] The time spent sending and receiving messages between processors instead of computing
- [ ] The complexity of the communication protocol
- [ ] The cost of communication hardware
- [ ] The error rate in communication

#### Q. What does "superlinear speedup" mean?

- [ ] Speedup that exceeds expectations
- [x] Speedup greater than the number of processors (greater than linear), often due to cache effects
- [ ] The maximum possible speedup
- [ ] Speedup using special hardware
- [ ] A theoretical impossibility

#### Q. What is the "efficiency paradox" in parallel computing?

- [ ] Parallel programs being less efficient
- [x] As you add more processors, efficiency typically decreases even as speedup increases
- [ ] Sequential programs being more efficient
- [ ] A bug in efficiency calculations
- [ ] The cost of efficiency optimizations

#### Q. How does problem size affect parallel speedup?

- [ ] Larger problems always have lower speedup
- [x] Larger problems often achieve better speedup because parallel overhead becomes a smaller fraction of total work
- [ ] Problem size doesn't affect speedup
- [ ] Smaller problems always have better speedup
- [ ] Only medium-sized problems benefit from parallelization

#### Q. What is "profiling" in the context of parallel performance?

- [ ] Creating user profiles
- [x] Measuring and analyzing where a program spends its time to identify bottlenecks
- [ ] Profiling hardware capabilities
- [ ] Creating performance profiles for different users
- [ ] A security analysis technique

#### Q. What is the "roofline model"?

- [ ] A model for ceiling height calculations
- [x] A visual performance model that shows the upper bound on performance based on compute and memory bandwidth
- [ ] A model for network topology
- [ ] A hardware architecture model
- [ ] A cost estimation model

#### Q. What is "Karp-Flatt metric"?

- [ ] A metric for memory usage
- [x] A metric to measure the serial fraction of a parallel program empirically
- [ ] A metric for network latency
- [ ] A metric for cache efficiency
- [ ] A metric for power consumption

#### Q. What is "synchronization overhead"?

- [ ] The cost of buying synchronization hardware
- [x] Time spent waiting at synchronization points (barriers, locks) instead of computing
- [ ] The complexity of synchronization code
- [ ] Network synchronization delays
- [ ] Clock synchronization time

#### Q. What is "scalability analysis"?

- [ ] Analyzing code size
- [x] Studying how performance changes as the number of processors or problem size increases
- [ ] Analyzing hardware capabilities
- [ ] Measuring memory scalability
- [ ] A type of security analysis

#### Q. What is "performance portability"?

- [ ] Running the same code on different systems
- [x] Achieving good performance across different hardware architectures without major code changes
- [ ] Porting code between languages
- [ ] Moving performance data between systems
- [ ] Portable performance monitoring

#### Q. What is "idle time" in parallel computing?

- [ ] Time when the computer is in sleep mode
- [x] Time when a processor is not doing useful work while waiting for data or synchronization
- [ ] Time between program runs
- [ ] Time to initialize the system
- [ ] Time to shut down the system

#### Q. What is "memory latency"?

- [ ] The size of memory
- [x] The time delay between requesting data from memory and receiving it
- [ ] The speed of memory
- [ ] The age of memory hardware
- [ ] Memory allocation time

#### Q. What is "arithmetic intensity"?

- [ ] How complex the math operations are
- [x] The ratio of floating-point operations to bytes of data moved from memory
- [ ] The speed of arithmetic operations
- [ ] The number of arithmetic units
- [ ] A measure of CPU capability
