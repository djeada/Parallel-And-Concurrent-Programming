# Performance Evaluation Quiz

Notes: [Evaluating Performance](../notes/07_evaluating_performance.md), [Designing Parallel Programs](../notes/08_designing_parallel_programs.md)

Check your understanding of speedup, efficiency, scaling laws, work and span, and how to measure parallel programs.

#### Q. Which of these describes a program's latency?

* [ ] The ratio of sequential to parallel execution time
* [ ] The number of processors the program uses
* [ ] The number of tasks completed per unit of time
* [x] The time one task takes to complete

> **Explanation:** Latency is measured per operation, from start to finish. Percentiles such as p95 and p99 are often reported because an average can hide slow outliers.

#### Q. A server is always busy processing exactly 4 requests at a time, and each request takes 200 ms. What is its throughput?

* [ ] 5 requests per second
* [x] 20 requests per second
* [ ] 200 requests per second
* [ ] 800 requests per second

> **Explanation:** Each of the 4 slots completes $\frac{1}{0.2\,\text{s}} = 5$ requests per second, giving 20 in total. Latency is still 200 ms: concurrency raised throughput without making any single request faster.

#### Q. Which of these describes a program's speedup?

* [x] One-processor time divided by $p$-processor time
* [ ] The number of processors the program uses
* [ ] The number of tasks completed per unit of time
* [ ] The time one task takes to complete

> **Explanation:** Speedup is $S_p = \frac{T_1}{T_p}$, based on wall-clock time for the same problem. A speedup of $p$ is linear, or ideal, speedup.

#### Q. 85% of a program's runtime is parallelizable and runs 6 times faster on a 6-core processor. According to Amdahl's Law, what is the overall speedup?

* [x] About 3.43
* [ ] About 4.62
* [ ] About 5.10
* [ ] Exactly 6.00

> **Explanation:** $S = \frac{1}{(1 - 0.85) + \frac{0.85}{6}} = \frac{1}{0.15 + 0.142} \approx 3.43$. The 15% serial portion keeps the result well below the core count.

#### Q. Amdahl's Law gives a(n) _____ on the overall speedup from parallelizing a program.

* [ ] exact prediction
* [ ] lower limit
* [ ] average value
* [x] upper limit

> **Explanation:** The model assumes the parallel part divides perfectly and ignores communication, synchronization and imbalance. Real programs therefore usually do worse; superlinear cache effects are an exception, because they change the per-processor work the model assumes.

#### Q. If 95% of a fixed-size program can be parallelized, what is the maximum speedup with unlimited processors, according to Amdahl's Law?

* [ ] 5
* [ ] 19
* [x] 20
* [ ] 95

> **Explanation:** As the processor count grows, the speedup approaches $\frac{1}{1 - P} = \frac{1}{0.05} = 20$. The 5% serial part takes the same time no matter how many processors are added.

#### Q. 95% of a fixed-size program's runtime is perfectly parallelizable. According to Amdahl's Law, how many processors are needed for a speedup of 10?

* [ ] 10
* [ ] 11
* [x] 19
* [ ] 20

> **Explanation:** Solve $\frac{1}{0.05 + 0.95/n} = 10$: then $0.05 + \frac{0.95}{n} = 0.1$, so $n = 19$. Reaching half of the limit of 20 already takes 19 processors.

#### Q. A function accounts for 10% of a program's runtime. If it were made infinitely fast, what is the best possible overall speedup?

* [x] About 1.11
* [ ] About 1.9
* [ ] 10
* [ ] Unlimited

> **Explanation:** By Amdahl's Law, the other 90% is unchanged, so the speedup is at most $\frac{1}{0.9} \approx 1.11$. Optimization effort belongs where the measured runtime is.

#### Q. How does Gustafson's Law differ from Amdahl's Law?

* [ ] It applies only to shared-memory machines
* [ ] It gives a lower bound instead of an upper bound
* [ ] It assumes the serial fraction grows with $p$
* [x] It lets the problem grow with the processor count

> **Explanation:** Amdahl asks how much faster a fixed problem can run. Gustafson asks how much larger a problem can be solved in about the same time, giving a more optimistic, weak-scaling view.

#### Q. A scaled workload has a serial fraction of 0.05. What speedup does Gustafson's Law predict on 32 processors?

* [ ] About 12.5
* [ ] About 20.0
* [x] About 30.45
* [ ] Exactly 32.0

> **Explanation:** Gustafson's scaled speedup is $p - \alpha(p - 1) = 32 - 0.05 \times 31 = 30.45$. Because the parallel work grows with $p$, the serial part matters much less than under Amdahl's Law.

#### Q. With a serial fraction of 0.1 on 8 processors, Amdahl's Law predicts a speedup of about 4.7 but Gustafson's Law predicts 7.3. Why is this not a contradiction?

* [ ] Gustafson's Law ignores the serial part of the program entirely
* [x] Gustafson's fraction is measured on a run whose problem grows with $p$
* [ ] Amdahl's Law applies only to distributed-memory machines
* [ ] Gustafson's Law assumes the processors run at a higher clock speed

> **Explanation:** Amdahl gives $\frac{1}{0.1 + 0.9/8} \approx 4.71$ for a fixed problem. Gustafson gives $8 - 0.1 \times 7 = 7.3$, where 0.1 is the serial share of the larger, scaled parallel run. They answer different questions.

#### Q. A computation takes 100 seconds on one processor and 28 seconds on four. What is its parallel efficiency?

* [ ] About 28%
* [ ] About 72%
* [x] About 89%
* [ ] About 357%

> **Explanation:** Speedup is $\frac{100}{28} \approx 3.57$, and efficiency is speedup divided by processor count: $\frac{3.57}{4} \approx 0.89$. An efficiency near 100% means near-linear speedup.

#### Q. As more processors are applied to a fixed-size problem, what typically happens to parallel efficiency?

* [x] It falls, even while speedup may still increase
* [ ] It rises steadily along with speedup
* [ ] It becomes exactly zero beyond 8 processors
* [ ] It stays constant at 100%

> **Explanation:** Serial work, communication and synchronization stay roughly constant or grow while each processor's share of useful work shrinks, so each added processor contributes less.

#### Q. What is superlinear speedup, and what can cause it?

* [ ] An impossible result that always indicates a timing bug
* [ ] Speedup below 1, caused by too much communication
* [ ] Speedup exactly equal to $p$, from perfect load balance
* [x] Speedup above $p$, often because each part now fits in cache

> **Explanation:** Splitting the problem shrinks each worker's working set, which can cut cache misses or paging. The parallel run then does less memory work per element than the one-processor baseline.

#### Q. What is an advantage of fine-grained parallelism with many small tasks?

* [ ] A high computation-to-communication ratio
* [ ] Lower scheduling overhead
* [ ] Better cache locality
* [x] Good load balancing

> **Explanation:** Many small tasks can be spread evenly and rebalanced dynamically. The cost is more frequent scheduling, synchronization and communication.

#### Q. What is an advantage of coarse-grained parallelism with a few large tasks?

* [ ] Good load balancing for irregular work
* [x] A high computation-to-communication ratio
* [ ] More tasks available than processors
* [ ] Easier recovery from slow tasks

> **Explanation:** Large tasks do more computation between coordination events, so overhead is a smaller fraction of runtime. If a few tasks run long, though, other processors sit idle.

#### Q. Granularity can be described as the ratio of _____ to _____.

* [x] computation; communication
* [ ] speedup; number of processors
* [ ] tasks; number of processors
* [ ] tasks; time

> **Explanation:** Granularity compares useful work in a task with its coordination cost. A task must do enough work to amortise the cost of scheduling and communicating it.

#### Q. In a computational graph, which of these describes the critical path?

* [x] The longest chain of dependent operations through the program
* [ ] The total time of all task nodes in the graph
* [ ] The shortest path from the start node to the end node
* [ ] The set of tasks that access shared memory

> **Explanation:** Operations on the critical path must run one after another, so its length bounds the execution time no matter how many processors are available.

#### Q. Task A (2 s) must finish before tasks B (3 s) and C (5 s) can start, and task D (1 s) needs both B and C. What are the work and span?

* [ ] Work 11 s, span 6 s
* [x] Work 11 s, span 8 s
* [ ] Work 8 s, span 11 s
* [ ] Work 11 s, span 11 s

> **Explanation:** Work sums every task: $2 + 3 + 5 + 1 = 11$ s. Span is the longest dependency path, A, C, D: $2 + 5 + 1 = 8$ s. The parallelism, $\frac{11}{8} \approx 1.4$, caps the speedup however many processors are used.

#### Q. In a computational graph, which of these describes a program's span?

* [ ] The total time of all task nodes in the graph
* [ ] The time between the first and second task nodes
* [ ] The number of processors needed to run the graph
* [x] The total time of the task nodes along the critical path

> **Explanation:** Span, $T_\infty$, is the execution time with unlimited processors. It equals the length of the critical path.

#### Q. What does the ratio of work to span represent?

* [ ] The communication overhead paid per processor
* [x] The parallelism, an upper bound on achievable speedup
* [ ] The fraction of the program that must run serially
* [ ] The number of tasks that are on the critical path

> **Explanation:** Because span is part of the work, the ratio is always at least 1. A ratio of 1 means no parallelism; larger values mean more processors can be used profitably.

#### Q. A computation has work $T_1 = 1000$ s and span $T_\infty = 50$ s. What lower bound do the work and span laws give for its runtime on 8 processors?

* [ ] 20 s
* [ ] 50 s
* [x] 125 s
* [ ] 175 s

> **Explanation:** $T_p \ge \max\left(\frac{T_1}{p}, T_\infty\right) = \max(125, 50) = 125$ s. A greedy scheduler needs at most $\frac{T_1}{p} + T_\infty = 175$ s.

#### Q. Why are computational graphs useful when designing a parallel program?

* [x] They show which tasks depend on which, exposing parallelism
* [ ] They automatically eliminate data races between tasks
* [ ] They replace the need to measure performance later
* [ ] They pinpoint exactly which lines of code need a mutex

> **Explanation:** Tasks with no path between them in the graph are independent and can run concurrently. The graph also exposes the critical path that limits speedup.

#### Q. Increasing the number of processors while keeping the problem size per processor fixed (weak scaling) aims to accomplish _____ in _____.

* [ ] the same work; less time
* [ ] less work; the same time
* [ ] more work; less time
* [x] more work; the same time

> **Explanation:** In weak scaling the total problem grows with the machine. Ideally the runtime stays constant while the total amount of work increases.

#### Q. Increasing the number of processors while keeping the total problem size fixed (strong scaling) aims to accomplish _____ in _____.

* [ ] more work; less time
* [x] the same work; less time
* [ ] more work; the same time
* [ ] less work; more time

> **Explanation:** Strong scaling divides a fixed problem among more processors. It eventually saturates when each processor's share becomes too small relative to overhead.

#### Q. In a weak-scaling study, one unit of work takes 10 s on 1 processor, and 64 units take 12.5 s on 64 processors. What is the weak-scaling efficiency $E_{\text{weak}} = \frac{T_1}{T_p}$?

* [ ] About 1.3%
* [ ] 64%
* [x] 80%
* [ ] 125%

> **Explanation:** $\frac{10}{12.5} = 0.8$. In ideal weak scaling the runtime stays constant as work and processors grow together, giving an efficiency of 1.

#### Q. A parallel code on 1 thread runs twice as slowly as the best sequential program. On 8 threads it runs 6 times faster than on 1 thread. What is its speedup over the best sequential program?

* [ ] 2
* [x] 3
* [ ] 6
* [ ] 12

> **Explanation:** $T_{\text{par}}(1) = 2T_s$, so $T_{\text{par}}(8) = \frac{2T_s}{6} = \frac{T_s}{3}$. Reporting only the relative speedup of 6 would hide the parallel version's overhead, so always state the baseline.

#### Q. Why should you repeat a timing measurement several times and report the variation?

* [ ] The system clock drifts randomly between runs
* [ ] The first run always crashes on parallel machines
* [x] Scheduling, caches and background load vary between runs
* [ ] Averaging makes the program itself run faster

> **Explanation:** Scheduling, cache state, frequency scaling and background activity all change runtimes. Repeated runs, with warm-up handled consistently, show typical behavior and how much it varies.

#### Q. A report compares a serial run's end-to-end time, including loading the input, with a parallel run's compute-phase time only. What is wrong?

* [ ] Nothing, as long as both runs use the same input data
* [ ] A compute phase cannot be timed inside a parallel program
* [ ] Serial programs must always be timed using CPU time
* [x] The timed regions differ, so the reported speedup is inflated

> **Explanation:** The comparison credits parallelism with removing input loading that it never sped up. Time the same region in both runs, and report excluded startup or transfers separately.

#### Q. A parallel run on 4 processors takes 30 seconds, and the one-processor baseline takes 100 seconds. What is the total parallel overhead, $T_o = p \cdot T_p - T_1$?

* [ ] 0 seconds
* [x] 20 seconds
* [ ] 70 seconds
* [ ] 120 seconds

> **Explanation:** The parallel run consumes $4 \times 30 = 120$ processor-seconds, while the useful work is 100 seconds. The extra 20 seconds are communication, synchronization, idle time and other overhead.

#### Q. What does the scalability of a parallel program describe?

* [x] How its performance changes as resources are added
* [ ] The maximum number of threads its language allows
* [ ] How large its source code can grow over time
* [ ] How many users can download and install it at once

> **Explanation:** A scalable program keeps gaining useful performance as resources grow. Scalability is measured experimentally, for example with strong- and weak-scaling studies.

#### Q. What is load imbalance?

* [x] Uneven work, so some processors wait while others are busy
* [ ] Too much total work for the machine's memory to hold
* [ ] A power supply that cannot sustain the processors' load
* [ ] A network link carrying more traffic than another

> **Explanation:** In synchronized programs the slowest worker sets the runtime. Dynamic scheduling, work stealing or finer-grained tasks can improve balance.

#### Q. Four workers finish a phase in 10, 10, 10 and 30 seconds. What is the imbalance ratio, $T_{\max} / T_{\text{avg}}$?

* [ ] 1.0
* [ ] 1.5
* [x] 2.0
* [ ] 3.0

> **Explanation:** The average is $\frac{60}{4} = 15$ seconds, and $\frac{30}{15} = 2.0$. A ratio of 1.0 would mean perfect balance; here three workers sit idle for 20 seconds each.

#### Q. What does isoefficiency describe?

* [ ] Every processor in a system having equal efficiency
* [ ] The efficiency of a program's reads and writes to disk
* [ ] The energy used per operation by a parallel computer
* [x] How fast problem size must grow with $p$ to keep efficiency fixed

> **Explanation:** A slowly growing isoefficiency function means the algorithm scales well: modest increases in problem size keep added processors productive.

#### Q. A program uses 4 threads and finishes in 10 seconds of elapsed time, consuming about 40 seconds of CPU time. Which time should be used to compute its speedup?

* [ ] The CPU time of the slowest thread only
* [ ] The 40 seconds of CPU time
* [x] The 10 seconds of wall-clock time
* [ ] The CPU time divided by the number of threads

> **Explanation:** Speedup is about how long the user waits, which is elapsed wall-clock time. CPU time adds up across threads and can increase even as the program gets faster.

#### Q. Why do larger problems often achieve better parallel speedup than small ones?

* [ ] Larger problems always contain less serial code
* [x] Fixed overheads become a smaller share of the total work
* [ ] Larger problems run at a higher clock frequency
* [ ] Small problems cannot be split across processors at all

> **Explanation:** Communication and start-up costs matter relatively less as useful work per processor grows. This is the effect Gustafson's Law and weak scaling describe.

#### Q. What is the main purpose of profiling a parallel program?

* [x] To find where time goes, including waiting, before optimising
* [ ] To list every hardware platform the program supports
* [ ] To test the program for security vulnerabilities
* [ ] To create performance profiles for different users

> **Explanation:** In parallel programs, time is often lost in locks, barriers, communication or imbalance rather than in the hottest function. Measure first, then optimise the measured bottleneck.

#### Q. Dashboards show a cluster job's throughput dropping every night at 02:00. What is the usual next step?

* [ ] Add more nodes, since monitoring has already found the cause
* [x] Profile or trace the job during that period to find out why
* [ ] Stop monitoring the job, since collecting metrics adds overhead
* [ ] Raise the alert threshold so the nightly drop no longer alerts

> **Explanation:** Monitoring shows when and where a problem occurs; profiling and tracing the affected phase explain why, for example a competing backup job or one slow node.

#### Q. How does sampling profiling differ from instrumentation?

* [ ] Sampling requires recompiling the whole program; instrumentation never does
* [ ] They are two names for the same profiling technique
* [ ] Sampling can measure only GPU code; instrumentation only CPU code
* [x] Sampling records the location periodically; instrumentation adds measurement code

> **Explanation:** Tools such as `perf` sample, which barely disturbs timing. Instrumentation, such as the call counting `gprof` adds with `-pg` or Callgrind's binary instrumentation, records every call but can change the behavior it measures.

#### Q. What does the roofline model show?

* [ ] The network topology connecting the nodes of a cluster
* [ ] The physical height limit of racks in a data centre
* [x] Performance bounds set by peak compute and memory bandwidth
* [ ] The cost of a machine relative to its performance

> **Explanation:** It plots attainable performance against arithmetic intensity. Kernels with low intensity sit under the sloped memory-bandwidth roof; those with high intensity hit the flat compute roof. It shows which limit to attack.

#### Q. What is arithmetic intensity?

* [x] Operations performed per byte moved from memory
* [ ] How complicated a program's mathematical formulas are
* [ ] The clock speed of the arithmetic units
* [ ] The number of arithmetic units in a processor

> **Explanation:** High arithmetic intensity means each byte loaded is reused for much computation, so the code is more likely to be compute-bound than memory-bound.

#### Q. When timing an MPI phase, why is the maximum elapsed time across ranks reported, for example with `MPI_Reduce` and `MPI_MAX`?

* [ ] The maximum is always the most accurate single measurement
* [ ] Rank 0 cannot measure its own time
* [x] The phase is not complete until the slowest rank finishes
* [ ] MPI timers only work on the slowest rank

> **Explanation:** Averaging per-rank times would hide stragglers. The parallel phase ends when every rank has finished, so the slowest one determines its duration.

#### Q. A program reports 100% CPU utilisation on every core but shows poor speedup. How is this possible?

* [ ] It is not possible, since full utilisation guarantees linear speedup
* [ ] Speedup cannot be measured when utilisation is 100%
* [ ] Utilisation counters are always inaccurate on multicore CPUs
* [x] Cores may be busy spinning on locks, missing in cache or redoing work

> **Explanation:** Busy is not the same as productive. Interpret utilisation together with throughput, efficiency and hardware counters to see whether the cycles are doing useful work.

#### Q. Why can timing only the host call that launches a GPU kernel be misleading?

* [ ] Kernel launches always take exactly the same time as the kernel
* [x] The launch returns before the kernel finishes running
* [ ] GPU kernels cannot be timed from the host at all
* [ ] The host clock is paused while a kernel runs

> **Explanation:** The timer captures only submission. Use GPU events for device time, or synchronize before stopping a host timer. Also state whether data transfers and allocation are included.

#### Q. In a strong-scaling study, what is the "scaling knee"?

* [ ] The processor count at which the program first produces correct output
* [ ] The point where efficiency first exceeds 100%
* [ ] The processor count at which power consumption is highest
* [x] The point beyond which adding processors gives little further improvement

> **Explanation:** Plotting runtime, speedup and efficiency against processor count shows where gains flatten out. Running past the knee wastes resources for that workload and configuration.
