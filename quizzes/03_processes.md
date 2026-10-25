# Multiprocessing Quiz

Notes: [Multiprocessing](../notes/03_multiprocessing.md)

Check your understanding of process lifecycles, inter-process communication, load balancing and Python's `multiprocessing` module.

#### Q. In a producer-consumer architecture, how should the average consumption rate relate to the average production rate?

* [ ] The consumption rate should be lower than the production rate
* [x] The consumption rate should be greater than or equal to the production rate
* [ ] The two rates must be exactly equal at every single moment
* [ ] The rates do not matter as long as the buffer is large enough

> **Explanation:** If consumers are slower on average, the buffer grows without bound or producers stall forever. Short bursts are absorbed by the buffer, but long-run consumption must keep up.

#### Q. Which architecture consists of a chain of producer-consumer pairs?

* [x] Pipeline
* [ ] Client-server
* [ ] Peer-to-peer
* [ ] Master-worker

> **Explanation:** In a pipeline each stage consumes the previous stage's output and produces input for the next, such as download, then parse, then write.

#### Q. What is a key advantage of using processes instead of threads?

* [ ] Processes share all their memory with each other by default
* [ ] Processes are cheaper to create and switch between
* [ ] Processes never need any synchronization for shared files
* [x] A crash or stray write in one process cannot corrupt another's private memory

> **Explanation:** Each process normally has its own address space, so a stray write or crash stays contained. The cost is higher start-up overhead and explicit IPC for sharing data.

#### Q. In Python's `multiprocessing`, how do the `fork` and `spawn` start methods differ?

* [x] `fork` copies the parent process; `spawn` starts a fresh Python interpreter
* [ ] `fork` is available only on Windows, and `spawn` only on Linux
* [ ] `fork` creates a thread in the parent; `spawn` creates a process
* [ ] `fork` shares the parent's memory for writes; `spawn` copies it

> **Explanation:** A forked child inherits a snapshot of the parent's memory. A spawned child starts a new interpreter and receives its target and arguments by pickling, so start-up is slower.

#### Q. Starting with Python 3.14, which start method does `multiprocessing` use by default on Linux?

* [ ] `fork`
* [ ] `spawn`
* [x] `forkserver`
* [ ] `threads`

> **Explanation:** Python 3.14 stopped defaulting to `fork` everywhere. Windows and macOS use `spawn`, and other POSIX platforms such as Linux use `forkserver`; Python 3.13 and earlier used `fork` on Linux.

#### Q. A script calls `mp.Process(target=f).start()` at module level, with no `if __name__ == "__main__":` guard. It works on Python 3.12 on Linux but fails with `RuntimeError` on Python 3.14. Why?

* [ ] Python 3.14 forbids creating processes outside a function
* [ ] Python 3.14 requires every target function to be declared `async`
* [x] The 3.14 default, `forkserver`, makes the child import the main module, re-running the start-up code
* [ ] The 3.12 child was a thread, while the 3.14 child is a real process

> **Explanation:** With `spawn` or `forkserver` the child imports the main module as `__mp_main__`, so unguarded code tries to start a process during bootstrapping and Python raises `RuntimeError`. `fork` copies the parent instead, which hid the bug.

#### Q. A script sets `CONFIG["mode"] = "changed"` inside `if __name__ == "__main__":` and then starts a `multiprocessing.Process` whose target prints `CONFIG["mode"]`. What does the child print with the default start method on Linux?

```python
CONFIG = {"mode": "default"}

def show():
    print(CONFIG["mode"])

if __name__ == "__main__":
    CONFIG["mode"] = "changed"
    p = multiprocessing.Process(target=show)
    p.start()
    p.join()
```

* [ ] `changed` on every Python version
* [ ] `default` on every Python version
* [x] `changed` on Python 3.12 (`fork`), but `default` on Python 3.14 (`forkserver`)
* [ ] `default` on Python 3.12, but `changed` on Python 3.14

> **Explanation:** A forked child copies the parent's current memory. A `spawn` or `forkserver` child re-imports the module, which re-runs `CONFIG = {...}` but not the guarded block; pass needed state as arguments instead.

#### Q. A Python 3.12 program on Linux passes a `lambda` as the `target` of a `multiprocessing.Process`. What happens?

* [ ] It fails with every start method, because a target must be a named function
* [ ] It works with every start method, because the target runs in the parent
* [ ] It works with `spawn` but fails with `fork`
* [x] It works with `fork`, but `spawn` and `forkserver` raise a pickling error

> **Explanation:** A forked child already has the function in its copied memory. `spawn` and `forkserver` must pickle the target, and a lambda cannot be pickled by reference, so define workers at module level.

#### Q. A Python 3.12 program starts a background thread and then creates a `multiprocessing.Process` with the default `fork` method on Linux. Why does Python emit a `DeprecationWarning`?

* [ ] The child process inherits every thread, so the work is done twice
* [x] Only the forking thread is copied, so a lock held by another thread can stay locked in the child
* [ ] `fork()` cannot be called while any thread is running and fails
* [ ] Threads and processes cannot exist in the same Python program

> **Explanation:** After `fork()` the child has a single thread, and locks other threads held at that moment are never released, so the child can deadlock. That risk is one reason Python 3.14 stopped using `fork` by default.

#### Q. When is shared memory preferable to message passing between processes?

* [ ] When the processes run on different machines in a cluster
* [x] When same-host processes swap large data with low latency
* [ ] When you want to avoid any need for synchronization
* [ ] When messages must survive a crash of either process

> **Explanation:** Shared memory avoids copying data through an IPC channel. It only works on one machine and requires explicit synchronization of the shared region.

#### Q. What is a `multiprocessing.Queue` used for?

* [ ] Ordering processes by priority for the scheduler
* [ ] Limiting how many processes can exist
* [ ] Storing the PIDs of child processes
* [x] Passing picklable objects safely between processes

> **Explanation:** The queue serializes objects and handles the underlying pipe and locking, so processes can exchange messages without sharing ordinary Python objects.

#### Q. A child process puts a 10 MB object on a `multiprocessing.Queue`. The parent calls `p.join()` before `q.get()`. What happens?

* [x] The join can hang, because the child waits for its queued data to be consumed before exiting
* [ ] The join returns at once, because `put()` copied the data into the parent
* [ ] The data is dropped when the child exits, so `q.get()` blocks forever
* [ ] Python raises `RuntimeError`, because joining before reading is forbidden

> **Explanation:** A child that has put items on a queue does not terminate until its feeder thread has flushed them into the pipe. Get the results first, then join.

#### Q. Which `multiprocessing` classes store simple numeric data in shared memory that several processes can update?

* [ ] `Queue` and `Pipe`
* [ ] `Pool` and `Process`
* [x] `Value` and `Array`
* [ ] `Event` and `Barrier`

> **Explanation:** `Value('i', 0)` and `Array('d', 10)` live in shared memory, so all processes see the same data. Queues and pipes copy messages instead.

#### Q. Why does this worker wrap the update in `get_lock()`?

```python
def increment(shared_value):
    with shared_value.get_lock():
        shared_value.value += 1
```

* [ ] `Value` objects cannot be read at all without holding a lock
* [ ] The lock copies the current value into the child process
* [ ] It is required to keep the process from becoming a zombie
* [x] `+=` is a read-modify-write, so concurrent updates could be lost

> **Explanation:** The read, add and write steps can interleave across processes; four processes doing 50,000 unlocked increments each can end far below 200,000. `get_lock()` returns the lock that protects the value, making each update exclusive.

#### Q. What does `multiprocessing.Pool` provide?

* [x] A set of reusable worker processes that run tasks in parallel
* [ ] A pool of shared variables that processes can lock
* [ ] A memory pool that reduces allocation overhead for processes
* [ ] A registry of every process currently running on the system

> **Explanation:** A pool starts its workers once and distributes many tasks to them, which avoids the cost of creating a process per task.

#### Q. In Python's `multiprocessing.Pool`, how does `map()` differ from `apply_async()`?

* [x] `map()` blocks for all results; `apply_async()` returns at once
* [ ] `map()` runs one call; `apply_async()` runs a list of calls
* [ ] `map()` uses threads; `apply_async()` uses processes
* [ ] `map()` is deprecated; `apply_async()` replaces it

> **Explanation:** `map()` applies a function to every item and returns the result list once all are done. `apply_async()` schedules one call and lets you collect its result later with `.get()`.

#### Q. A function submitted with `pool.apply_async(f)` raises `ValueError`. When does the parent process see the error?

* [ ] Never, because exceptions in workers are silently discarded
* [ ] Immediately, because the whole pool shuts down with it
* [ ] Only in the worker's log output, as a printed traceback
* [x] When it calls `.get()`, which re-raises the `ValueError`

> **Explanation:** The pool pickles the exception and sends it back with the result. `.get()` re-raises it in the parent, so errors are handled where the result is used.

#### Q. A `multiprocessing.Process` target raises `ValueError`. What happens when the parent calls `p.join()`?

* [ ] `join()` re-raises the `ValueError` in the parent
* [x] `join()` returns normally, and `p.exitcode` is `1`
* [ ] `join()` blocks forever because the child crashed
* [ ] The parent process is terminated as well

> **Explanation:** A plain `Process` does not send exceptions back to the parent; the child just prints a traceback. Check `exitcode` after `join()`, or report errors through a queue or use a pool or executor.

#### Q. What is a zombie process?

* [x] A finished child whose exit status has not been collected
* [ ] A process that is still running but no longer responds
* [ ] A process that keeps consuming memory after it is killed
* [ ] A process that ignores every signal, including `SIGTERM`

> **Explanation:** A zombie no longer runs, but its process-table entry remains until the parent reads its exit status. Many zombies can exhaust PIDs or process-table capacity.

#### Q. A C parent forks a child that returns immediately, then sleeps without calling `wait()`. What does `ps -o stat=,cmd= -p <child-pid>` show on Linux during that sleep?

* [ ] Nothing, because the child's entry was removed when it exited
* [ ] `S` (sleeping), because the child is waiting for its parent
* [x] `Z` and `<defunct>`, because the child is a zombie
* [ ] `T` (stopped), because the child is waiting to be reaped

> **Explanation:** The exited child keeps its process-table entry until the parent reaps it, and `ps` marks it `Z` with `<defunct>`. Calling `wait()` or `waitpid()` removes it.

#### Q. How does a parent process prevent its finished children from remaining zombies?

* [ ] By sending each child `SIGKILL` after it exits
* [ ] By closing its standard output before forking
* [ ] By lowering each child's priority with `nice`
* [x] By calling `wait()` or `waitpid()` for each child

> **Explanation:** Waiting reaps the child and removes its process-table entry. In Python, `Process.join()` does this for you.

#### Q. What does this C program print after reaping its child?

```c
int main(void) {
    pid_t p = fork();
    if (p == 0)
        exit(3);
    int status;
    waitpid(p, &status, 0);
    printf("%d %d\n", WIFEXITED(status), WEXITSTATUS(status));
}
```

* [x] `1 3`
* [ ] `0 3`
* [ ] `3 0`
* [ ] `1 0`

> **Explanation:** `waitpid()` stores an encoded status. `WIFEXITED` is true because the child exited normally, and `WEXITSTATUS` extracts the exit code `3`.

#### Q. On a UNIX-like system, what happens to a running child process when its parent terminates?

* [x] It keeps running and is reparented to PID 1 or a subreaper
* [ ] The kernel terminates the child along with its parent
* [ ] The child becomes a zombie until the system reboots
* [ ] The child is paused until a new parent claims it

> **Explanation:** The orphan continues normally. Its new parent collects its exit status when it finishes, so it does not stay a zombie.

#### Q. In Python's `multiprocessing`, how does `Process.terminate()` differ from `Process.kill()` on POSIX systems?

* [ ] `terminate()` pauses the process; `kill()` stops it
* [x] `terminate()` sends `SIGTERM`; `kill()` sends `SIGKILL`
* [ ] `terminate()` is for daemons; `kill()` is for all processes
* [ ] They are identical aliases for the same method

> **Explanation:** With Python's default handling, both end the child without running `finally` blocks, and `exitcode` becomes `-15` or `-9`. A process can install a `SIGTERM` handler, but `SIGKILL` cannot be caught.

#### Q. A process reads from a pipe until EOF, but it never sees EOF even after the writer finishes. What is the most likely cause?

* [ ] The pipe's buffer is too small for all the data written
* [ ] Pipes never report EOF, so the reader must count the bytes itself
* [ ] The writer used `print()` instead of `os.write()`
* [x] A copy of the write end is still open somewhere, maybe in the reader

> **Explanation:** A reader sees EOF only after every write endpoint is closed. Inherited, unused pipe ends must be closed in both parent and child.

#### Q. A process writes two 100-byte messages to a pipe. What can the reader's first `read(4096)` return?

* [ ] Exactly the first 100-byte message, every single time
* [ ] Nothing at all until the writer closes its end of the pipe
* [x] Any amount, such as part of a message or both messages joined together
* [ ] The second message, because pipes are last-in, first-out

> **Explanation:** A pipe carries a byte stream with no message boundaries. Use fixed-size records, a length prefix or a delimiter to split it back into messages.

#### Q. What is a named pipe (FIFO) on a POSIX system?

* [ ] A pipe whose data is labelled with a message name by the sender
* [x] A pipe with a filesystem name that unrelated processes can open
* [ ] A faster pipe that bypasses the kernel's buffers
* [ ] A pipe that can only carry newline-terminated text

> **Explanation:** An anonymous pipe is shared only through inheritance, usually between parent and child. A FIFO has a path, so any process with permission can open it.

#### Q. What does setting `daemon=True` on a `multiprocessing.Process` do?

* [ ] Runs the process with elevated, administrator-level privileges
* [ ] Makes the process immune to signals such as `SIGTERM`
* [x] Makes the parent terminate the child when the parent exits normally
* [ ] Lets the process share memory with other daemons

> **Explanation:** Python's exit handling terminates daemonic children instead of waiting for them; if the parent is killed by `SIGKILL`, the child keeps running as an orphan. A daemonic process may not create child processes.

#### Q. On an 8-core machine, which approach best speeds up CPU-bound pure-Python work in CPython 3.12?

* [ ] A `ThreadPoolExecutor` with 8 worker threads
* [ ] An `asyncio` event loop running 8 tasks
* [ ] A single thread with a higher OS priority
* [x] A `ProcessPoolExecutor` with 8 worker processes

> **Explanation:** Each process has its own interpreter and GIL, so CPU-bound Python code can run in parallel across cores. Threads and asyncio in the default GIL build take turns on one GIL.

#### Q. What is copy-on-write in the context of `fork()`?

* [ ] The child receives a full copy of the parent's memory before it starts
* [x] Parent and child share memory pages until one of them writes to a page
* [ ] Writes in the child are copied back into the parent
* [ ] Files are copied when a forked process writes to them

> **Explanation:** Copying is deferred until a write occurs, which makes `fork()` fast. Logically the memories are still separate: a write is never seen by the other process.

#### Q. What does a `multiprocessing.Manager` provide?

* [ ] Control over process priorities and CPU affinity
* [ ] Automatic restart of worker processes that crash
* [ ] A way to allocate raw shared memory pages directly
* [x] A server process that holds shared objects behind proxies

> **Explanation:** Manager proxies let several processes modify shared dicts, lists and other objects. Every access goes through the manager process, which is slower than `Value` or `Array`.

#### Q. What happens to open file descriptors when a process calls `fork()`?

* [ ] They are closed automatically in the child
* [x] The child inherits copies referring to the same open files
* [ ] They become invalid in both processes until reopened
* [ ] The child receives read-only versions of them

> **Explanation:** Inherited descriptors share the same underlying open files, including file offsets. That is why unused pipe ends must be closed in each process.

#### Q. What does `os.fork()` return?

* [ ] The child's PID in both processes
* [ ] `0` in the parent and the parent's PID in the child
* [x] `0` in the child and the child's PID in the parent
* [ ] `True` if the fork succeeded, otherwise `False`

> **Explanation:** Both processes continue from the same call, so the return value tells them apart. A negative value is never returned in Python; failures raise `OSError`.

#### Q. How many lines does this C program print?

```c
int main(void) {
    for (int i = 0; i < 3; i++)
        fork();
    printf("x\n");
    while (wait(NULL) > 0) {}
}
```

* [ ] 3
* [ ] 4
* [ ] 6
* [x] 8

> **Explanation:** Every existing process forks in each iteration, doubling the count: $2^3 = 8$ processes reach the `printf`.

#### Q. This program's output is redirected to a file with `./a.out > out.txt`. What does `out.txt` contain?

```c
int main(void) {
    printf("hello\n");
    fork();
    while (wait(NULL) > 0) {}
}
```

* [x] `hello` twice, because the unflushed buffer is copied into the child
* [ ] `hello` once, because the `printf` runs before `fork()`
* [ ] Nothing, because the child closes the shared file
* [ ] `hello` once, followed by an error from the child

> **Explanation:** Output to a file is fully buffered, so `hello` is still in the stdio buffer at `fork()` and both processes flush it at exit. On a terminal stdout is line-buffered and it prints once; call `fflush(stdout)` before forking.

#### Q. What does this C program print, assuming `echo` is on the `PATH`?

```c
int main(void) {
    printf("before\n");
    fflush(stdout);
    execlp("echo", "echo", "replaced", (char *)NULL);
    printf("after\n");
    return 1;
}
```

* [ ] `before`, `replaced`, then `after`
* [ ] `before`, then `after`
* [x] `before`, then `replaced`
* [ ] `replaced` only

> **Explanation:** A successful `exec` replaces the process image, keeping the PID, so the code after it never runs. Only if `execlp` fails does it return and print `after`.

#### Q. What value of `x` does the parent in this C program print?

```c
int x = 1;

int main(void) {
    pid_t p = fork();
    if (p == 0) {
        x = 42;
        return 0;
    }
    waitpid(p, NULL, 0);
    printf("%d\n", x);
}
```

* [x] `1`
* [ ] `42`, because forked processes share global variables
* [ ] `42` or `1`, depending on which process runs first
* [ ] `0`, because `fork()` resets globals in the parent

> **Explanation:** After `fork()` parent and child have logically separate memory, so the child's write is never seen by the parent. Use IPC or explicitly shared memory such as `mmap(MAP_SHARED)` to pass changes.

#### Q. What is process (CPU) affinity?

* [ ] A measure of how well two processes cooperate
* [x] Restricting a process to run on a chosen set of CPU cores
* [ ] The amount of memory shared between processes
* [ ] The parent-child relationship between processes

> **Explanation:** Pinning a process to cores, for example with `taskset -c 0-7`, can improve cache and NUMA locality and reduce migration. The scheduler then only places it on the allowed cores.

#### Q. How does a signal differ from a message in IPC?

* [ ] Signals carry more data than messages do
* [ ] Messages can only be exchanged between threads
* [x] A signal is a bare notification; a message carries data
* [ ] Signals need a shared memory region to work

> **Explanation:** A signal such as `SIGTERM` tells a process that an event occurred but carries little information. Messages through queues, pipes or sockets transfer actual data.

#### Q. A pool of 8 database connections is shared by many worker processes. Which primitive bounds the number of workers using a connection at once?

* [ ] A mutex
* [ ] A barrier for 8 parties
* [ ] A condition variable with no predicate
* [x] A counting semaphore initialised to 8

> **Explanation:** Each worker acquires the semaphore before taking a connection and releases it afterwards. When all 8 slots are in use, further workers wait.

#### Q. A bounded buffer with N slots is implemented with semaphores `empty`, `full` and a mutex `m`. What are their initial values?

* [x] `empty = N`, `full = 0`, `m = 1`
* [ ] `empty = 0`, `full = N`, `m = 1`
* [ ] `empty = N`, `full = N`, `m = 0`
* [ ] `empty = 1`, `full = 1`, `m = N`

> **Explanation:** Initially every slot is free and no item is available. Producers wait on `empty`, consumers wait on `full`, and the binary semaphore `m` starts unlocked.

#### Q. Every process acquires resources in the same global order: DB, then Cache, then Log. Which deadlock condition does this break?

* [ ] Mutual exclusion
* [ ] Hold and wait
* [ ] No preemption
* [x] Circular wait

> **Explanation:** If everyone requests resources in the same order, no process can hold a later resource while waiting for an earlier one, so a waiting cycle cannot form.

#### Q. In a simple readers-preference reader-writer lock, what is the main risk?

* [ ] Readers can corrupt the data by reading concurrently
* [x] Writers can starve if new readers keep arriving
* [ ] Two writers can write at the same time
* [ ] Readers deadlock whenever a writer is waiting

> **Explanation:** The lock lets new readers join while any reader is active, so a waiting writer may never get exclusive access. Writer-preference or fair locks address this.

#### Q. What is a memory-mapped file, and how can it be used for IPC?

* [x] A file mapped into memory, which several processes can share
* [ ] A file kept only in RAM and never written to disk
* [ ] A compressed copy of a process's memory saved to disk
* [ ] A log of every memory access that a process makes

> **Explanation:** With `mmap` and `MAP_SHARED`, processes that map the same file see the same pages. Like other shared memory, access still needs synchronization.

#### Q. Two worker processes split 10 tasks statically: the first five go to A and the last five to B. Every task takes 50 ms except the last, which takes 500 ms. When are both done?

* [ ] After about 475 ms, half of the total work
* [ ] After about 500 ms, the length of the longest task
* [x] After about 700 ms, when worker B finishes
* [ ] After about 950 ms, the sum of all the tasks

> **Explanation:** Worker A needs $5 \times 50 = 250$ ms, but B needs $4 \times 50 + 500 = 700$ ms. Dynamic scheduling from a shared queue can cut this to about 500 ms if the long task starts early.

#### Q. In a work-stealing scheduler, where does an idle worker take tasks from?

* [ ] From a single global queue shared by all workers
* [ ] From the same end of a busy worker's deque that the owner uses
* [x] From the opposite end of a busy worker's deque to the one the owner uses
* [ ] From whichever worker has the shortest queue, after checking them all

> **Explanation:** The owner pushes and pops at one end (LIFO, good for cache locality), while thieves steal from the other end. Operations are mostly local, so contention stays low.

#### Q. In "power of two choices" load balancing, where does a new task go?

* [ ] To the worker with the shortest queue among all workers
* [x] To the less loaded of two randomly sampled workers
* [ ] To two workers, keeping whichever result arrives first
* [ ] To workers in strict round-robin order

> **Explanation:** Sampling just two workers is cheap yet greatly reduces the maximum queue length compared with purely random assignment.

#### Q. A program spends 20% of its run time in code that cannot be parallelised. By Amdahl's law, what speedup can it reach on 8 processors?

* [ ] $8\times$
* [ ] $6.4\times$
* [ ] $5\times$
* [x] $3.33\times$

> **Explanation:** $\frac{1}{0.2 + 0.8/8} = \frac{1}{0.3} \approx 3.33$. Even with unlimited processors the speedup cannot exceed $1/0.2 = 5$, because the sequential part never shrinks.

#### Q. Two processes repeatedly increment different `int` fields of one struct in shared memory, and the program scales badly. What is a likely cause and fix?

* [ ] The fields race with each other, so each needs its own mutex
* [x] False sharing of one cache line; align each field to its own cache line
* [ ] Shared memory is uncached, so copy the struct into a pipe instead
* [ ] The kernel serializes all writes to shared pages, so use threads

> **Explanation:** Adjacent fields often share a 64-byte cache line, which bounces between cores on every write although the data is independent. Padding or `alignas(64)` gives each field its own line.

#### Q. What is the difference between vertical and horizontal scaling?

* [x] Vertical adds CPU or RAM to one machine; horizontal adds more machines
* [ ] Vertical adds more machines; horizontal adds CPU or RAM to one machine
* [ ] Vertical scales storage; horizontal scales network bandwidth
* [ ] Vertical uses threads; horizontal uses processes

> **Explanation:** Scaling up is simpler but limited by the largest available machine. Scaling out, for example behind a load balancer, goes further but adds distribution and failure handling.

#### Q. What is the main difference in isolation between containers and virtual machines?

* [ ] Containers each run their own kernel; VMs share the host kernel
* [x] Containers share the host kernel; each VM runs its own guest OS kernel
* [ ] Containers cannot run more than one process; VMs can run many
* [ ] Containers isolate nothing; VMs isolate only the filesystem

> **Explanation:** Containers isolate processes with kernel features such as namespaces, so they start quickly and stay light. A VM boots a full guest OS on a hypervisor, which costs more but isolates more strongly.

#### Q. What does Node.js's `child_process.fork()` do?

* [ ] It calls the POSIX `fork()` system call to duplicate the Node.js process
* [ ] It creates a worker thread that shares the parent's heap
* [x] It starts a new Node.js process running a module, with an IPC channel for `send()` and `'message'`
* [ ] It runs a shell command and buffers its whole output

> **Explanation:** Despite the name, Node's `fork()` spawns a fresh Node.js process rather than duplicating the current one. `exec()` is the helper that runs a shell command and buffers its output.
