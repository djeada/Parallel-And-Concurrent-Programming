# Multiprocessing Quiz

Notes: [Multiprocessing](../notes/03_multiprocessing.md)

Check your understanding of process lifecycles, inter-process communication and Python's `multiprocessing` module.

#### Q. In a producer-consumer architecture, how should the average consumption rate relate to the average production rate?

* [ ] The consumption rate should be lower than the production rate
* [x] The consumption rate should be greater than or equal to the production rate
* [ ] The two rates must be exactly equal at every single moment
* [ ] The rates do not matter as long as the buffer is large enough

> **Explanation:** If consumers are slower on average, the buffer grows without bound or producers stall forever. Short bursts are absorbed by the buffer, but long-run consumption must keep up.

#### Q. Which architecture consists of a chain of producer-consumer pairs?

* [ ] Client-server
* [x] Pipeline
* [ ] Peer-to-peer
* [ ] Master-worker

> **Explanation:** In a pipeline each stage consumes the previous stage's output and produces input for the next, such as download, then parse, then write.

#### Q. In most operating systems, what decides when each thread and process gets to run on a CPU?

* [ ] The processor hardware, on its own
* [x] The operating system's scheduler
* [ ] The application, by calling `yield` explicitly
* [ ] The compiler, when it generates the code

> **Explanation:** The OS scheduler picks which ready thread runs on each core and when to preempt it. Applications can influence it with priorities or affinity but do not control it.

#### Q. What is a key advantage of using processes instead of threads?

* [ ] Processes share all their memory with each other by default
* [ ] Processes are cheaper to create and switch between
* [x] One process crashing cannot corrupt another's memory
* [ ] Processes never need any synchronization for shared files

> **Explanation:** Each process normally has its own address space, so a stray write or crash stays contained. The cost is higher start-up overhead and explicit IPC for sharing data.

#### Q. In Python's `multiprocessing`, how do the `fork` and `spawn` start methods differ?

* [ ] `fork` is available only on Windows, and `spawn` only on Linux
* [x] `fork` copies the parent process; `spawn` starts a fresh Python interpreter
* [ ] `fork` creates a thread in the parent; `spawn` creates a process
* [ ] `fork` shares the parent's memory for writes; `spawn` copies it

> **Explanation:** A forked child inherits a snapshot of the parent's memory. A spawned child starts a new interpreter and receives its target and arguments by pickling, so start-up is slower.

#### Q. Starting with Python 3.14, which start method does `multiprocessing` use by default on Linux?

* [ ] `fork`
* [ ] `spawn`
* [x] `forkserver`
* [ ] `vfork`

> **Explanation:** Python 3.14 stopped defaulting to `fork` everywhere. Windows and macOS use `spawn`, and other POSIX platforms such as Linux use `forkserver`; earlier versions used `fork` on Linux.

#### Q. Why should code that creates processes be placed under `if __name__ == "__main__":`?

* [ ] It makes the child processes start faster on every platform
* [x] Spawned children re-import the main module, re-running unguarded code
* [ ] Python refuses to create processes outside a function
* [ ] It stops finished child processes from becoming zombies

> **Explanation:** A spawned child re-imports the main module to find the target function. Without the guard, that import would execute the process-creation code again, possibly recursively.

#### Q. When is shared memory preferable to message passing between processes?

* [ ] When the processes run on different machines in a cluster
* [x] When same-host processes swap large data with low latency
* [ ] When you want to avoid any need for synchronization
* [ ] When messages must survive a crash of either process

> **Explanation:** Shared memory avoids copying data through an IPC channel. It only works on one machine and requires explicit synchronization of the shared region.

#### Q. What is a `multiprocessing.Queue` used for?

* [ ] Ordering processes by priority for the scheduler
* [ ] Limiting how many processes can exist
* [x] Passing picklable objects safely between processes
* [ ] Storing the PIDs of child processes

> **Explanation:** The queue serializes objects and handles the underlying pipe and locking, so processes can exchange messages without sharing ordinary Python objects.

#### Q. Which `multiprocessing` classes store simple numeric data in shared memory that several processes can update?

* [x] `Value` and `Array`
* [ ] `Queue` and `Pipe`
* [ ] `Pool` and `Process`
* [ ] `Event` and `Barrier`

> **Explanation:** `Value('i', 0)` and `Array('d', 10)` live in shared memory, so all processes see the same data. Queues and pipes copy messages instead.

#### Q. Why does this worker wrap the update in `get_lock()`?

```python
def increment(shared_value):
    with shared_value.get_lock():
        shared_value.value += 1
```

* [ ] `Value` objects cannot be read at all without holding a lock
* [x] `+=` is a read-modify-write, so concurrent updates could be lost
* [ ] The lock copies the current value into the child process
* [ ] It is required to keep the process from becoming a zombie

> **Explanation:** The read, add and write steps can interleave across processes, so updates can be lost. `get_lock()` returns the lock that protects the shared value, making the update exclusive.

#### Q. What does `multiprocessing.Pool` provide?

* [ ] A pool of shared variables that processes can lock
* [x] A set of reusable worker processes that run tasks in parallel
* [ ] A memory pool that reduces allocation overhead for processes
* [ ] A registry of every process currently running on the system

> **Explanation:** A pool starts its workers once and distributes many tasks to them, which avoids the cost of creating a process per task.

#### Q. In Python's `multiprocessing.Pool`, how does `map()` differ from `apply_async()`?

* [ ] `map()` runs one call; `apply_async()` runs a list of calls
* [x] `map()` blocks for all results; `apply_async()` returns at once
* [ ] `map()` uses threads; `apply_async()` uses processes
* [ ] `map()` is deprecated; `apply_async()` replaces it

> **Explanation:** `map()` applies a function to every item and returns the result list once all are done. `apply_async()` schedules one call and lets you collect its result later with `.get()`.

#### Q. A function submitted with `pool.apply_async(f)` raises `ValueError`. When does the parent process see the error?

* [ ] Never, because exceptions in workers are silently discarded
* [x] When it calls `.get()`, which re-raises the `ValueError`
* [ ] Immediately, because the whole pool shuts down with it
* [ ] Only in the worker's log output, as a printed traceback

> **Explanation:** The pool pickles the exception and sends it back with the result. `.get()` re-raises it in the parent, so errors are handled where the result is used.

#### Q. A `multiprocessing.Process` target raises `ValueError`. What happens when the parent calls `p.join()`?

* [ ] `join()` re-raises the `ValueError` in the parent
* [x] `join()` returns normally, and `p.exitcode` is `1`
* [ ] `join()` blocks forever because the child crashed
* [ ] The parent process is terminated as well

> **Explanation:** A plain `Process` does not send exceptions back to the parent; the child just prints a traceback. Check `exitcode` after `join()`, or report errors through a queue or use a pool or executor.

#### Q. Which of the following is NOT an inter-process communication mechanism?

* [ ] Pipes
* [ ] Shared memory
* [ ] Sockets
* [x] Thread-local storage

> **Explanation:** Thread-local storage gives each thread its own copy of data inside one process. Pipes, sockets and shared memory all move or share data across process boundaries.

#### Q. What is a zombie process?

* [ ] A process that is still running but no longer responds
* [x] A finished child whose exit status has not been collected
* [ ] A process that keeps consuming memory after it is killed
* [ ] A process that ignores every signal, including `SIGTERM`

> **Explanation:** A zombie no longer runs, but its process-table entry remains until the parent reads its exit status. Many zombies can exhaust PIDs or process-table capacity.

#### Q. How does a parent process prevent its finished children from remaining zombies?

* [ ] By sending each child `SIGKILL` after it exits
* [x] By calling `wait()` or `waitpid()` for each child
* [ ] By closing its standard output before forking
* [ ] By lowering each child's priority with `nice`

> **Explanation:** Waiting reaps the child and removes its process-table entry. In Python, `Process.join()` does this for you.

#### Q. On a UNIX-like system, what happens to a running child process when its parent terminates?

* [ ] The kernel terminates the child along with its parent
* [x] It keeps running and is reparented to PID 1 or a subreaper
* [ ] The child becomes a zombie until the system reboots
* [ ] The child is paused until a new parent claims it

> **Explanation:** The orphan continues normally. Its new parent collects its exit status when it finishes, so it does not stay a zombie.

#### Q. In Python's `multiprocessing`, how does `Process.terminate()` differ from `Process.kill()` on POSIX systems?

* [ ] `terminate()` pauses the process; `kill()` stops it
* [x] `terminate()` sends `SIGTERM`; `kill()` sends `SIGKILL`
* [ ] `terminate()` is for daemons; `kill()` is for all processes
* [ ] They are identical aliases for the same method

> **Explanation:** Both stop the process abruptly from Python's point of view, so `finally` blocks and cleanup may not run. `SIGTERM` can be handled by the process, while `SIGKILL` always ends it.

#### Q. A process reads from a pipe until EOF, but it never sees EOF even after the writer finishes. What is the most likely cause?

* [ ] The pipe's buffer is too small for all the data written
* [x] A copy of the write end is still open somewhere, maybe in the reader
* [ ] Pipes never report EOF, so the reader must count the bytes itself
* [ ] The writer used `print()` instead of `os.write()`

> **Explanation:** A reader sees EOF only after every write endpoint is closed. Inherited, unused pipe ends must be closed in both parent and child.

#### Q. A process writes two 100-byte messages to a pipe. What can the reader's first `read(4096)` return?

* [ ] Exactly the first 100-byte message, every single time
* [x] Any amount, such as part of a message or both messages joined together
* [ ] Nothing at all until the writer closes its end of the pipe
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
* [x] Makes the parent terminate the child when the parent exits
* [ ] Makes the process immune to signals such as `SIGTERM`
* [ ] Lets the process share memory with other daemons

> **Explanation:** Daemonic children are terminated when their parent exits, so they suit background helpers whose work can be abandoned. A daemonic process is not allowed to create child processes of its own.

#### Q. On an 8-core machine, which approach best speeds up CPU-bound pure-Python work in standard CPython?

* [ ] A `ThreadPoolExecutor` with 8 worker threads
* [ ] An `asyncio` event loop running 8 tasks
* [x] A `ProcessPoolExecutor` with 8 worker processes
* [ ] A single thread with a higher OS priority

> **Explanation:** Each process has its own interpreter and GIL, so CPU-bound Python code can run in parallel across cores. Threads and asyncio take turns on one GIL.

#### Q. What is copy-on-write in the context of `fork()`?

* [ ] The child receives a full copy of the parent's memory before it starts
* [x] Parent and child share memory pages until one of them writes to a page
* [ ] Writes in the child are copied back into the parent
* [ ] Files are copied when a forked process writes to them

> **Explanation:** Copying is deferred until a write occurs, which makes `fork()` fast. Logically the memories are still separate: a write is never seen by the other process.

#### Q. What does a `multiprocessing.Manager` provide?

* [ ] Control over process priorities and CPU affinity
* [ ] Automatic restart of worker processes that crash
* [x] A server process that holds shared objects behind proxies
* [ ] A way to allocate raw shared memory pages directly

> **Explanation:** Manager proxies let several processes modify shared dicts, lists and other objects. Every access goes through the manager process, which is slower than `Value` or `Array`.

#### Q. What happens to open file descriptors when a process calls `fork()`?

* [ ] They are closed automatically in the child
* [x] The child inherits copies referring to the same open files
* [ ] They become invalid in both processes until reopened
* [ ] The child receives read-only versions of them

> **Explanation:** Inherited descriptors share the same underlying open files, including file offsets. That is why unused pipe ends must be closed in each process.

#### Q. What does `os.fork()` return?

* [ ] The child's PID in both processes
* [x] `0` in the child and the child's PID in the parent
* [ ] `0` in the parent and the parent's PID in the child
* [ ] `True` if the fork succeeded, otherwise `False`

> **Explanation:** Both processes continue from the same call, so the return value tells them apart. A negative value is never returned in Python; failures raise `OSError`.

#### Q. What does a call from the POSIX `exec` family do?

* [ ] Creates a new child process running another program
* [x] Replaces the calling process's program image with a new program
* [ ] Starts a new thread inside the current process to run the program
* [ ] Duplicates the current process, exactly like `fork()` does

> **Explanation:** `exec` keeps the same process and PID but loads a new program. The usual way to start another program is `fork()` followed by `exec()` in the child.

#### Q. After `fork()`, the child sets an ordinary global variable `x = 42`. What does the parent see?

* [x] Its own unchanged value of `x`
* [ ] `42`, because forked processes share memory
* [ ] `42`, but only after the child exits
* [ ] An error, because the variable is locked

> **Explanation:** Parent and child have logically separate memory after `fork()`. Use IPC or explicitly shared memory to pass changes between them.

#### Q. What is process (CPU) affinity?

* [ ] A measure of how well two processes cooperate
* [x] Restricting a process to run on a chosen set of CPU cores
* [ ] The amount of memory shared between processes
* [ ] The parent-child relationship between processes

> **Explanation:** Pinning a process to cores can improve cache and NUMA locality and reduce migration. The scheduler then only places it on the allowed cores.

#### Q. How does a signal differ from a message in IPC?

* [ ] Signals carry more data than messages do
* [x] A signal is a bare notification; a message carries data
* [ ] Messages can only be exchanged between threads
* [ ] Signals need a shared memory region to work

> **Explanation:** A signal such as `SIGTERM` tells a process that an event occurred but carries little information. Messages through queues, pipes or sockets transfer actual data.

#### Q. A pool of 8 database connections is shared by many worker processes. Which primitive bounds the number of workers using a connection at once?

* [ ] A mutex
* [x] A counting semaphore initialised to 8
* [ ] A barrier for 8 parties
* [ ] A condition variable with no predicate

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

* [ ] A file kept only in RAM and never written to disk
* [x] A file mapped into memory, which several processes can share
* [ ] A compressed copy of a process's memory saved to disk
* [ ] A log of every memory access that a process makes

> **Explanation:** With `mmap`, processes that map the same file see the same pages. Like other shared memory, access still needs synchronization.

#### Q. In "power of two choices" load balancing, where does a new task go?

* [ ] To the worker with the shortest queue among all workers
* [x] To the less loaded of two randomly sampled workers
* [ ] To two workers, keeping whichever result arrives first
* [ ] To workers in strict round-robin order

> **Explanation:** Sampling just two workers is cheap yet greatly reduces the maximum queue length compared with purely random assignment.

#### Q. What is the difference between vertical and horizontal scaling?

* [x] Vertical adds CPU or RAM to one machine; horizontal adds more machines
* [ ] Vertical adds more machines; horizontal adds CPU or RAM to one machine
* [ ] Vertical scales storage; horizontal scales network bandwidth
* [ ] Vertical uses threads; horizontal uses processes

> **Explanation:** Scaling up is simpler but limited by the largest available machine. Scaling out, for example behind a load balancer, goes further but adds distribution and failure handling.
