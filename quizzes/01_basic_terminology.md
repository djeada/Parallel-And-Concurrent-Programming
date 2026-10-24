# Basic Terminology Quiz

Notes: [Basic Terminology](../notes/01_basic_terminology.md), [Multithreading](../notes/02_multithreading.md)

Check your understanding of concurrency, parallelism, processes, threads and the classic hazards of shared state.

#### Q. A _____ contains one or more _____.

* [ ] thread; processes
* [ ] thread; other threads
* [x] process; threads
* [ ] process; other processes

> **Explanation:** A thread is a path of execution inside a process, and every process has at least one thread. Child processes are separate processes, not parts of their parent.

#### Q. Which statement best distinguishes concurrency from parallelism?

* [x] Concurrency means tasks overlap in time; parallelism means they execute at the same instant
* [ ] Concurrency requires multiple cores; parallelism can be achieved on a single core
* [ ] Concurrency applies to I/O work; parallelism applies only to network work
* [ ] Concurrency and parallelism are two names for the same execution model

> **Explanation:** Concurrent tasks are all in progress over the same period, possibly by taking turns. Parallel tasks actually run simultaneously, which needs hardware such as multiple cores.

#### Q. On a single-core CPU without simultaneous multithreading, how can two tasks run?

* [x] Concurrently, by taking turns on the core
* [ ] In parallel, executing at the same instant
* [ ] Only sequentially, one finishing before the other starts
* [ ] Only if both tasks are separate processes

> **Explanation:** The scheduler can switch between the tasks so both make progress, which is concurrency. Executing both at the same instant (parallelism) needs more than one hardware execution context.

#### Q. What does parallel execution require that concurrent execution does not?

* [ ] An event loop that switches between pending tasks
* [x] Hardware that can run several operations at once
* [ ] Tasks written with `async`/`await` so they can yield
* [ ] A shared address space so the tasks can exchange data

> **Explanation:** Concurrency can be achieved by interleaving tasks on one core. Parallelism needs multiple execution resources, such as several CPU cores, so work truly runs simultaneously.

#### Q. A program starts threads A, B and C in that order. In which order are they guaranteed to run?

* [ ] A, then B, then C, matching the creation order
* [ ] C, then B, then A, because the newest thread runs first
* [x] No particular order; the scheduler decides
* [ ] In order of their thread IDs, lowest first

> **Explanation:** Thread scheduling is nondeterministic. If the order matters, it must be enforced with synchronization such as joins, events or condition variables.

#### Q. Which of these does each thread in a process have its own copy of?

* [ ] The heap
* [ ] Global and static variables
* [x] Its call stack
* [ ] The table of open files

> **Explanation:** Threads share the process's code, data, heap and open files. Each thread has its own stack, registers and program counter so it can follow its own path of execution.

#### Q. Compared with threads, processes _____.

* [ ] are simpler to communicate between
* [ ] are considered more "lightweight"
* [ ] are faster to switch between
* [x] require more overhead to create

> **Explanation:** Creating a process means setting up a separate address space and other resources. Threads reuse their process's memory, so they are cheaper to create and switch between.

#### Q. Two processes need to exchange data. What do they normally use?

* [ ] Ordinary global variables, since the OS shares them automatically
* [x] An IPC mechanism such as a pipe, socket or shared memory
* [ ] Thread-local storage, which the OS makes visible to every process
* [ ] The process table, which stores each process's variables

> **Explanation:** Processes have separate address spaces and cannot read each other's memory directly. They communicate through IPC mechanisms that the operating system provides.

#### Q. The operating system assigns each process a unique _____.

* [x] process identifier (PID)
* [ ] CPU core for its whole lifetime
* [ ] process name
* [ ] number of threads

> **Explanation:** The PID lets the OS track and manage each process. Several processes can share a name, and processes are not tied to one core unless affinity is set.

#### Q. A thread is waiting for data to arrive from a network socket. Which scheduling state is it in?

* [ ] Ready
* [ ] Running
* [x] Waiting (blocked)
* [ ] Terminated

> **Explanation:** A waiting or blocked thread cannot continue until an event occurs, such as input arriving. A ready thread could run immediately and is only waiting for CPU time.

#### Q. What characterises an asynchronous call compared with a synchronous one?

* [ ] It always runs the operation on another CPU core
* [x] The caller can continue and handle the result or error later
* [ ] It returns its result before the operation starts
* [ ] It prevents any other thread from running meanwhile

> **Explanation:** An asynchronous call lets the caller start work and receive the result later, for example via a future, callback or `await`. It does not by itself imply another core or thread.

#### Q. An async program starts a download and awaits it before starting the next one, for ten files. How do the downloads run?

* [x] Sequentially, one after another
* [ ] In parallel on separate cores
* [ ] Concurrently, all ten overlapping
* [ ] In a random order chosen by the event loop

> **Explanation:** Using async syntax does not create overlap by itself. Each download is awaited to completion before the next is started, so they still happen one at a time.

#### Q. A program calls a nonblocking read on a socket that has no data available. What happens?

* [ ] The thread sleeps until the kernel signals that data arrived
* [ ] The OS raises a fatal error and terminates the process
* [x] It returns at once, reporting that no data is ready
* [ ] The call waits for a fixed timeout and then retries

> **Explanation:** A nonblocking call never puts the thread to sleep. The program must retry later or ask for a readiness notification, whereas a blocking read would wait for data.

#### Q. A program downloads 100 files one after another and spends most of its time waiting for responses. Which change is most likely to speed it up?

* [ ] Buying a CPU with a higher clock speed
* [ ] Rewriting the parsing code with a faster algorithm
* [x] Overlapping the requests with async I/O or threads
* [ ] Splitting each file's checksum calculation across cores

> **Explanation:** The program is I/O-bound, so its time goes into waiting. Overlapping independent requests hides that waiting; faster computation helps little when the CPU is mostly idle.

#### Q. A single-threaded calculation keeps one of eight cores at 100% while total CPU usage shows about 12%. What kind of task is it?

* [x] CPU-bound, because the one core it uses is saturated
* [ ] I/O-bound, because overall CPU usage is low
* [ ] Memory-bound, because the CPU is not fully used
* [ ] Idle, because most cores have no work

> **Explanation:** A task is CPU-bound when computation limits its speed. Using only one core can make total utilisation look low even though that core is fully busy.

#### Q. A race condition _____ a data race.

* [ ] only occurs when there is
* [x] can occur independently of
* [ ] is just another name for
* [ ] is always caused by

> **Explanation:** A race condition is a logic error where correctness depends on timing or ordering. It can happen even when every access is properly locked, so no data race exists.

#### Q. Which situation is a data race?

* [ ] Two threads read the same shared variable at the same time
* [x] One thread writes a shared variable as another reads it, unsynchronized
* [ ] A single-threaded program competes with other processes for CPU time
* [ ] Several threads wait on the same condition variable for a signal

> **Explanation:** A data race needs concurrent accesses to the same location, at least one of them a write, without proper synchronization. Concurrent reads alone are safe.

#### Q. Why can a program with a race condition produce different results on different runs?

* [ ] The CPU randomly corrupts values held in shared memory
* [x] The interleaving of thread operations can differ each run
* [ ] The compiler generates different code on every run
* [ ] Threads run more slowly when the network is busy

> **Explanation:** Thread interleaving depends on scheduling, load and timing, which vary between runs. When correctness depends on that order, the output changes with it.

#### Q. What is the primary purpose of synchronization in concurrent programming?

* [ ] To make concurrent programs run faster on more cores
* [x] To coordinate shared access and ordering between tasks
* [ ] To reduce the memory each thread needs for its stack
* [ ] To improve the CPU's cache hit rate for shared data

> **Explanation:** Synchronization primitives such as mutexes, semaphores and condition variables prevent conflicting accesses and enforce ordering. They add overhead, so they rarely make code faster.

#### Q. What does "mutual exclusion" guarantee?

* [ ] Each thread receives an equal share of CPU time from the scheduler
* [ ] Two processes never share any memory pages with each other
* [x] Only one thread at a time executes a protected critical section
* [ ] A thread can never be preempted by the scheduler

> **Explanation:** Mutual exclusion, typically provided by a mutex, lets only one thread at a time enter the protected code. Others trying to enter must wait until it is released.

#### Q. What is a critical section?

* [ ] The `main` function, where every thread of the program starts
* [ ] Code that must run faster than the rest of the program
* [x] Code touching shared state that only one thread may run at a time
* [ ] Code that handles fatal errors before the program exits

> **Explanation:** A critical section touches shared mutable state, so concurrent execution could corrupt it. It is protected with a lock and should be kept as short as possible.

#### Q. What does it mean for code to be thread-safe?

* [ ] It runs only on a single dedicated thread chosen by the OS
* [ ] It never raises exceptions, whichever thread calls it
* [x] It stays correct when called from several threads at once
* [ ] It always runs at a higher priority than other code

> **Explanation:** Thread-safe code protects or avoids shared mutable state, so concurrent calls do not corrupt data or break its invariants.

#### Q. What is a deadlock?

* [ ] A thread that runs in an infinite loop and never yields
* [x] Threads each waiting forever for a resource another one holds
* [ ] A memory leak caused by threads that are never joined
* [ ] A crash caused by one thread accessing memory another freed

> **Explanation:** In a deadlock the threads are all blocked waiting on each other in a cycle. Nothing breaks the cycle, so they wait forever.

#### Q. Which conditions must all hold at the same time for a deadlock to occur?

* [x] Mutual exclusion, hold and wait, no preemption, and circular wait
* [ ] Mutual exclusion, preemption, starvation, and busy waiting
* [ ] Circular wait, priority inversion, preemption, and livelock
* [ ] Shared memory, multiple cores, preemption, and blocking I/O

> **Explanation:** These are the four Coffman conditions. Breaking any one of them, for example circular wait by acquiring locks in a global order, prevents deadlock.

#### Q. What is starvation?

* [ ] A thread that consumes so much memory that others cannot run
* [ ] A worker thread that has no more tasks left in its queue
* [x] A thread repeatedly denied a resource while others keep using it
* [ ] All threads blocked waiting for each other in a cycle

> **Explanation:** A starving thread could run but keeps losing out, for example to higher-priority threads or a stream of readers. Unlike deadlock, the rest of the system keeps making progress.

#### Q. What is priority inversion?

* [ ] The scheduler deliberately running low-priority tasks first
* [x] A high-priority task waiting on a resource held by a low-priority task
* [ ] Two tasks swapping their priorities after every time slice
* [ ] A task's priority value being stored in memory with the wrong sign

> **Explanation:** If the low-priority holder is itself preempted by medium-priority work, the high-priority task can wait a long time. Priority inheritance is a common fix.

#### Q. What is a context switch?

* [ ] Switching a program from one programming language to another
* [x] Saving one thread's state and restoring another's so it can run on the CPU
* [ ] Moving a thread's data from the heap onto its own stack
* [ ] Changing the scheduling priority of a thread that is running

> **Explanation:** The scheduler saves registers, the program counter and other state of the current thread, then loads another's. Switching between processes also changes the address space, which costs more.

#### Q. What is the difference between preemptive and cooperative multitasking?

* [x] Preemptive lets the scheduler interrupt a task; cooperative relies on tasks to yield voluntarily
* [ ] Preemptive runs only on multicore CPUs; cooperative runs only on single-core CPUs
* [ ] Preemptive is used for I/O-bound work; cooperative is used for CPU-bound work
* [ ] Preemptive requires separate processes; cooperative requires threads in one process

> **Explanation:** OS threads are usually preempted when their time slice ends. Coroutines in an event loop are cooperative: they switch only when a task yields, for example at an `await`.

#### Q. What makes an operation atomic?

* [ ] It compiles to exactly one machine instruction
* [ ] It always finishes in constant time
* [x] Other threads can never observe it partially completed
* [ ] It can never fail or raise an error

> **Explanation:** An atomic operation is indivisible from other threads' point of view: they see either the state before or after it. It is not necessarily a single instruction or fast.

#### Q. What does a memory model define in concurrent programming?

* [ ] The physical layout of RAM modules and channels on the motherboard
* [x] Rules for when, and in what order, writes become visible to other threads
* [ ] The algorithm the allocator uses to place objects on the heap
* [ ] The maximum amount of stack and heap memory a thread may use

> **Explanation:** Compilers and CPUs reorder memory operations. A language memory model, such as C++'s, states which orderings and visibility guarantees synchronization operations provide.

#### Q. What does a happens-before relationship between operation A and operation B guarantee?

* [ ] A appears earlier than B in the source code
* [x] The effects of A are visible to B
* [ ] A and B run on the same CPU core
* [ ] A finishes executing within one time slice of B

> **Explanation:** Happens-before is established by synchronization, such as unlocking and then locking the same mutex. Source order alone does not create it across threads.

#### Q. A concurrent program is deterministic when it _____.

* [ ] always takes exactly the same time to run on a given machine
* [x] gives the same result for the same input under any scheduling
* [ ] uses a fixed number of threads chosen at start-up
* [ ] cannot be preempted or interrupted by the operating system

> **Explanation:** Determinism concerns results, not timing. A deterministic concurrent program enforces every ordering its output depends on, so different interleavings give the same answer.
