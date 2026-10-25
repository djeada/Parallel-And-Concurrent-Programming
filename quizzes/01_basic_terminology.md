# Basic Terminology Quiz

Notes: [Basic Terminology](../notes/01_basic_terminology.md), [Multithreading](../notes/02_multithreading.md)

Check your understanding of concurrency, parallelism, processes, threads and the classic hazards of shared state.

#### Q. A _____ contains one or more _____.

* [x] process; threads
* [ ] thread; processes
* [ ] thread; other threads
* [ ] process; other processes

> **Explanation:** A thread is a path of execution inside a process, and every process has at least one thread. Child processes are separate processes, not parts of their parent.

#### Q. Which statement best distinguishes concurrency from parallelism?

* [x] Concurrency means tasks overlap in time; parallelism means they execute at the same instant
* [ ] Concurrency requires multiple cores; parallelism can be achieved on a single core
* [ ] Concurrency applies to I/O work; parallelism applies only to network work
* [ ] Concurrency and parallelism are two names for the same execution model

> **Explanation:** Concurrent tasks are all in progress over the same period, possibly by taking turns. Parallel tasks actually run simultaneously, which needs hardware such as multiple cores.

#### Q. On a single-core CPU without simultaneous multithreading, how can two tasks run?

* [ ] In parallel, executing at the same instant
* [ ] Only sequentially, one finishing before the other starts
* [x] Concurrently, by taking turns on the core
* [ ] Only if both tasks are separate processes

> **Explanation:** The scheduler can switch between the tasks so both make progress, which is concurrency. Executing both at the same instant (parallelism) needs more than one hardware execution context.

#### Q. What does parallel execution require that concurrent execution does not?

* [ ] An event loop that switches between pending tasks
* [x] Hardware that can run several operations at once
* [ ] Tasks written with `async`/`await` so they can yield
* [ ] A shared address space so the tasks can exchange data

> **Explanation:** Concurrency can be achieved by interleaving tasks on one core. Parallelism needs multiple execution resources, such as several CPU cores, so work truly runs simultaneously.

#### Q. Two threads each make a blocking, synchronous HTTP request, and their waits for the responses overlap. How is this program best described?

* [ ] Sequential, because each call blocks until its response arrives
* [x] Concurrent, even though each individual call is synchronous
* [ ] Asynchronous, because two threads are involved
* [ ] Parallel only if the responses arrive at exactly the same instant

> **Explanation:** Synchronous describes how each caller waits; concurrency describes whether several tasks are in progress over the same period. Here both requests are in progress at once, so the program is concurrent.

#### Q. A program reads two independent files and then combines their contents. Which ordering constraint must it respect?

* [ ] The first file must be read completely before the second read starts
* [ ] Both reads must run in parallel on separate cores
* [ ] The combine step can start as soon as either read finishes
* [x] Both reads must finish before combining; the reads may run in any order

> **Explanation:** Independent tasks can be reordered or overlapped, but a dependent step must wait for every result it uses. This is non-sequential execution with one dependency.

#### Q. A program starts threads A, B and C in that order. In which order are they guaranteed to run?

* [ ] A, then B, then C, matching the creation order
* [x] No particular order; the scheduler decides
* [ ] C, then B, then A, because the newest thread runs first
* [ ] In order of their thread IDs, lowest first

> **Explanation:** Thread scheduling is nondeterministic. If the order matters, it must be enforced with synchronization such as joins, events or condition variables.

#### Q. Which of these does each thread in a process have its own copy of?

* [x] Its call stack
* [ ] The heap
* [ ] Global and static variables
* [ ] The table of open files

> **Explanation:** Threads share the process's code, data, heap and open files. Each thread has its own stack, registers and program counter so it can follow its own path of execution.

#### Q. Compared with threads, processes _____.

* [x] require more overhead to create
* [ ] are simpler to communicate between
* [ ] are considered more "lightweight"
* [ ] are faster to switch between

> **Explanation:** Creating a process means setting up a separate address space and other resources. Threads reuse their process's memory, so they are cheaper to create and switch between.

#### Q. Two processes need to exchange data. What do they normally use?

* [ ] Ordinary global variables, since the OS shares them automatically
* [x] An IPC mechanism such as a pipe, socket or shared memory
* [ ] Thread-local storage, which the OS makes visible to every process
* [ ] The process table, which stores each process's variables

> **Explanation:** Processes have separate address spaces and cannot read each other's memory directly. They communicate through IPC mechanisms that the operating system provides.

#### Q. The operating system assigns each process a unique _____.

* [ ] CPU core for its whole lifetime
* [ ] process name
* [x] process identifier (PID)
* [ ] number of threads

> **Explanation:** The PID lets the OS track and manage each process. Several processes can share a name, and processes are not tied to one core unless affinity is set.

#### Q. A thread is waiting for data to arrive from a network socket. Which scheduling state is it in?

* [ ] Ready
* [ ] Running
* [ ] Terminated
* [x] Waiting

> **Explanation:** A waiting (blocked) thread cannot continue until an event occurs, such as input arriving. A ready thread could run immediately and is only waiting for CPU time.

#### Q. In a process with two kernel-scheduled threads, thread 1 blocks in a `read()` on a socket. What can thread 2 do meanwhile?

* [x] Keep running, because blocking affects only the calling thread
* [ ] Nothing, because a blocking call suspends every thread in the process
* [ ] Run only after the OS converts the read into a nonblocking call
* [ ] Run only if it belongs to a different process

> **Explanation:** Scheduling states apply to individual threads. While thread 1 waits for data, thread 2 is still ready or running, which is how threads overlap I/O with other work.

#### Q. A process is paused at a breakpoint by a debugger. Which description fits its state?

* [ ] Waiting, exactly like a process blocked on disk I/O
* [ ] Zombie, because it is no longer executing
* [x] Stopped, a suspension distinct from waiting for an event
* [ ] Ready, because it could run as soon as a core is free

> **Explanation:** A stopped process will not run until it is explicitly resumed, for example with `SIGCONT`, even if CPU time is available. A blocked process resumes when its event occurs, and a zombie has already terminated.

#### Q. What characterises an asynchronous call compared with a synchronous one?

* [ ] It always runs the operation on another CPU core
* [x] The caller can continue and handle the result or error later
* [ ] It returns its result before the operation starts
* [ ] It prevents any other thread from running meanwhile

> **Explanation:** An asynchronous call lets the caller start work and receive the result later, for example via a future, callback or `await`. It does not by itself imply another core or thread.

#### Q. An async program starts a download and awaits it before starting the next one, for ten files. How do the downloads run?

* [ ] In parallel on separate cores
* [ ] Concurrently, all ten overlapping
* [x] Sequentially, one after another
* [ ] In a random order chosen by the event loop

> **Explanation:** Using async syntax does not create overlap by itself. Each download is awaited to completion before the next is started, so they still happen one at a time.

#### Q. A program calls a nonblocking read on a socket that has no data available. What happens?

* [ ] The thread sleeps until the kernel signals that data arrived
* [ ] The OS raises a fatal error and terminates the process
* [ ] The call waits for a fixed timeout and then retries
* [x] It returns at once, reporting that no data is ready

> **Explanation:** On POSIX, `read()` on an `O_NONBLOCK` socket fails with `EAGAIN` instead of sleeping. The program must retry later or ask for a readiness notification, whereas a blocking read would wait for data.

#### Q. A program downloads 100 files one after another and spends most of its time waiting for responses. Which change is most likely to speed it up?

* [ ] Buying a CPU with a higher clock speed
* [x] Overlapping the requests with async I/O or threads
* [ ] Rewriting the parsing code with a faster algorithm
* [ ] Splitting each file's checksum calculation across cores

> **Explanation:** The program is I/O-bound, so its time goes into waiting. Overlapping independent requests hides that waiting; faster computation helps little when the CPU is mostly idle.

#### Q. A downloader that overlaps its requests stops getting faster beyond 20 concurrent requests, while its CPU usage stays low. What is the most likely limit?

* [ ] The CPU, because one core cannot issue more than 20 requests
* [ ] The event loop, which cannot track more than 20 tasks
* [x] The network link or the remote server, which is now saturated
* [ ] The operating system's limit of 20 threads per process

> **Explanation:** Overlapping hides waiting, but the disk, network or remote service still caps how much I/O can complete per second. Once that resource is saturated, more concurrency adds no throughput.

#### Q. A single-threaded calculation keeps one of eight cores at 100% while total CPU usage shows about 12%. What kind of task is it?

* [x] CPU-bound, because the one core it uses is saturated
* [ ] I/O-bound, because overall CPU usage is low
* [ ] Memory-bound, because the CPU is not fully used
* [ ] Idle, because most cores have no work

> **Explanation:** A task is CPU-bound when computation limits its speed. Using only one core can make total utilisation look low even though that core is fully busy.

#### Q. A 10-millisecond calculation is split into 8 pieces on 8 cores, and the program becomes slower. What is the most likely cause?

* [x] Creating, coordinating and joining the workers costs more than the work saved
* [ ] Parallel code always runs slower than sequential code
* [ ] The CPU lowers its clock speed whenever more than one core is busy
* [ ] The pieces must run one after another because they share a process

> **Explanation:** Parallelism has overhead for starting workers, distributing data and combining results. For small amounts of work that overhead can outweigh the speedup, so measure before parallelising.

#### Q. A server moves from one core to four, handling four independent requests at the same time. What does parallelism mainly improve here?

* [ ] The time each individual request takes from start to finish
* [x] Throughput, the number of requests completed per second
* [ ] The amount of memory each request needs
* [ ] The order in which the requests are completed

> **Explanation:** Running independent requests simultaneously completes more work per unit of time. A single request still runs on one core, so its own latency is not reduced.

#### Q. A race condition _____ a data race.

* [ ] only occurs when there is
* [x] can occur independently of
* [ ] is just another name for
* [ ] is always caused by

> **Explanation:** A race condition is a logic error where correctness depends on timing or ordering. It can happen even when every access is properly locked, so no data race exists.

#### Q. Two threads run `buyer()` while `tickets` starts at `1`. Each access holds the mutex. What is wrong?

```cpp
std::mutex m;
int tickets = 1;

bool available() { std::lock_guard<std::mutex> g(m); return tickets > 0; }
void take()      { std::lock_guard<std::mutex> g(m); --tickets; }

void buyer() {
    if (available()) take();
}
```

* [ ] Nothing, because every access to `tickets` is protected by the mutex
* [ ] It deadlocks, because `take()` locks a mutex that `available()` still holds
* [ ] It has a data race, so the C++ program has undefined behavior
* [x] Both threads can pass the check before either takes, leaving `tickets` at `-1`

> **Explanation:** There is no data race, since every access is locked, but the check and the update are separate critical sections. This check-then-act race condition is fixed by holding one lock across both steps.

#### Q. Which situation is a data race?

* [ ] Two threads read the same shared variable at the same time
* [ ] A single-threaded program competes with other processes for CPU time
* [x] One thread writes a shared variable as another reads it, unsynchronized
* [ ] Several threads wait on the same condition variable for a signal

> **Explanation:** A data race needs concurrent accesses to the same location, at least one of them a write, without proper synchronization. Concurrent reads alone are safe.

#### Q. Why can a program with a race condition produce different results on different runs?

* [ ] The CPU randomly corrupts values held in shared memory
* [ ] The compiler generates different code on every run
* [ ] Threads run more slowly when the network is busy
* [x] The interleaving of thread operations can differ each run

> **Explanation:** Thread interleaving depends on scheduling, load and timing, which vary between runs. When correctness depends on that order, the output changes with it.

#### Q. What is the primary purpose of synchronization in concurrent programming?

* [x] To coordinate shared access and ordering between tasks
* [ ] To make concurrent programs run faster on more cores
* [ ] To reduce the memory each thread needs for its stack
* [ ] To improve the CPU's cache hit rate for shared data

> **Explanation:** Synchronization primitives such as mutexes, semaphores and condition variables prevent conflicting accesses and enforce ordering. They add overhead, so they rarely make code faster.

#### Q. What does "mutual exclusion" guarantee?

* [ ] Each thread receives an equal share of CPU time from the scheduler
* [ ] Two processes never share any memory pages with each other
* [ ] A thread can never be preempted by the scheduler
* [x] Only one thread at a time executes a protected critical section

> **Explanation:** Mutual exclusion, typically provided by a mutex, lets only one thread at a time enter the protected code. Others trying to enter must wait until it is released.

#### Q. What is a critical section?

* [x] Code accessing shared state that must not run in several threads at once
* [ ] The `main` function, where every thread of the program starts
* [ ] Code that must run faster than the rest of the program
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

* [ ] Mutual exclusion, preemption, starvation, and busy waiting
* [x] Mutual exclusion, hold and wait, no preemption, and circular wait
* [ ] Circular wait, priority inversion, preemption, and livelock
* [ ] Shared memory, multiple cores, preemption, and blocking I/O

> **Explanation:** These are the four Coffman conditions. Breaking any one of them, for example circular wait by acquiring locks in a global order, prevents deadlock.

#### Q. What is starvation?

* [ ] A thread that consumes so much memory that others cannot run
* [ ] A worker thread that has no more tasks left in its queue
* [ ] All threads blocked waiting for each other in a cycle
* [x] A thread repeatedly denied a resource while others keep using it

> **Explanation:** A starving thread could run but keeps losing out, for example to higher-priority threads or a stream of readers. Unlike deadlock, the rest of the system keeps making progress.

#### Q. What is priority inversion?

* [x] A high-priority task waiting on a resource held by a low-priority task
* [ ] The scheduler deliberately running low-priority tasks first
* [ ] Two tasks swapping their priorities after every time slice
* [ ] A task's priority value being stored in memory with the wrong sign

> **Explanation:** If the low-priority holder is itself preempted by medium-priority work, the high-priority task can wait a long time. Priority inheritance is a common fix.

#### Q. What is a context switch?

* [ ] Switching a program from one programming language to another
* [ ] Moving a thread's data from the heap onto its own stack
* [x] Saving one thread's state and restoring another's so it can run on the CPU
* [ ] Changing the scheduling priority of a thread that is running

> **Explanation:** The scheduler saves registers, the program counter and other state of the current thread, then loads another's. Switching between processes also changes the address space, which costs more.

#### Q. What is the difference between preemptive and cooperative multitasking?

* [ ] Preemptive runs only on multicore CPUs; cooperative runs only on single-core CPUs
* [ ] Preemptive is used for I/O-bound work; cooperative is used for CPU-bound work
* [x] Preemptive lets the scheduler interrupt a task; cooperative relies on tasks to yield voluntarily
* [ ] Preemptive requires separate processes; cooperative requires threads in one process

> **Explanation:** OS threads are usually preempted when their time slice ends. Coroutines in an event loop are cooperative: they switch only when a task yields, for example at an `await`.

#### Q. What makes an operation atomic?

* [ ] It compiles to exactly one machine instruction
* [ ] It always finishes in constant time
* [ ] It can never fail or raise an error
* [x] Other threads can never observe it partially completed

> **Explanation:** An atomic operation is indivisible from other threads' point of view: they see either the state before or after it. It is not necessarily a single instruction or fast.

#### Q. What does a memory model define in concurrent programming?

* [ ] The physical layout of RAM modules and channels on the motherboard
* [x] Rules for when, and in what order, writes become visible to other threads
* [ ] The algorithm the allocator uses to place objects on the heap
* [ ] The maximum amount of stack and heap memory a thread may use

> **Explanation:** Compilers and CPUs reorder memory operations. A language memory model, such as C++'s, states which orderings and visibility guarantees synchronization operations provide.

#### Q. What does a happens-before relationship between operation A and operation B guarantee?

* [ ] A appears earlier than B in the source code
* [ ] A and B run on the same CPU core
* [x] The effects of A are visible to B
* [ ] A finishes executing within one time slice of B

> **Explanation:** Happens-before is established by synchronization, such as unlocking and then locking the same mutex. Source order alone does not create it across threads.

#### Q. In this C++ program, is `main` guaranteed to print `1`?

```cpp
int x = 0;

int main() {
    std::thread t([] { x = 1; });
    t.join();
    std::cout << x << '\n';
}
```

* [ ] No, `x` must be `std::atomic<int>` for the write to become visible
* [ ] No, the program has a data race on `x`
* [ ] Only on x86, which keeps stores in program order
* [x] Yes, the thread's completion happens before `join()` returns

> **Explanation:** The end of a thread synchronizes with the return of `join()`, so the write to `x` happens before the read and there is no data race. ThreadSanitizer reports nothing for this program.

#### Q. A concurrent program is deterministic when it _____.

* [ ] always takes exactly the same time to run on a given machine
* [ ] uses a fixed number of threads chosen at start-up
* [x] gives the same result for the same input under any scheduling
* [ ] cannot be preempted or interrupted by the operating system

> **Explanation:** Determinism concerns results, not timing. A deterministic concurrent program enforces every ordering its output depends on, so different interleavings give the same answer.
