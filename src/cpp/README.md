# C++ examples: start with modern C++

Use **C++20** for the main learning path. Choose an algorithm or task abstraction
first, and manage worker lifetimes and locks through scoped ownership. C++11/17
facilities such as futures, mutexes, and execution policies remain useful in
modern code; their age alone does not make them legacy.

The topic directories contain the main examples. Older implementations retained
for comparison live in [`multithreading/legacy/`](multithreading/legacy/).
Files named `_antipattern.cpp` or `_antipatterns.cpp` deliberately demonstrate
problems. Low-level and platform-specific examples have their own purposes,
described below.

## Choose a solution by the problem

| Problem | Starting point | Example |
|---|---|---|
| Launch an external program | `posix_spawnp`, explicit file actions, and checked child status | [spawn_process.cpp](multiprocessing/spawn_process.cpp) |
| Reuse isolated workers | Persistent child processes, bounded in-flight work, and result pipes | [process_pool.cpp](multiprocessing/process_pool.cpp) |
| Share data between processes | Owned shared mapping and POSIX process-shared synchronization | [shared_memory.cpp](multiprocessing/shared_memory.cpp), [process_mutex.cpp](multiprocessing/process_mutex.cpp) |
| Distribute a calculation across ranks | MPI collectives through the supported C interface | [reduce_operation.cpp](mpi/reduce_operation.cpp) |
| Distribute uneven matrix partitions | `MPI_Scatterv` / `MPI_Gatherv` and contiguous buffers | [parallel_matrix_multiplication.cpp](mpi/parallel_matrix_multiplication.cpp) |
| Fetch several HTTP resources | Bounded libcurl multi event loop | [parallel_fetch.cpp](asynchrony/parallel_fetch.cpp) |
| Serve many waiting TCP connections | Owned nonblocking sockets and a bounded `poll` loop | [distributed_computing.cpp](asynchrony/distributed_computing.cpp) |
| Sort or reduce independent data | Standard algorithms with an execution policy | [parallel_algorithms.cpp](multithreading/parallel_algorithms.cpp) |
| Own a few worker threads | `std::jthread`; scope exit requests stop and joins | [jthread.cpp](multithreading/jthread.cpp) |
| Stop long-lived workers | `std::stop_token` and interruptible waits | [thread_life_cycle.cpp](multithreading/thread_life_cycle.cpp) |
| Run periodic work | Stop-aware timed waits | [scheduler.cpp](multithreading/scheduler.cpp) |
| Pause, resume, and cancel work | `condition_variable_any` with a stop token and predicate | [pause_resume.cpp](asynchrony/pause_resume.cpp) |
| Protect related state | `std::scoped_lock` or `std::lock_guard` | [simple_mutex.cpp](multithreading/simple_mutex.cpp) |
| Acquire several mutexes | One `std::scoped_lock` over all mutexes | [scoped_lock_deadlock_avoidance.cpp](multithreading/scoped_lock_deadlock_avoidance.cpp) |
| Count independent events | Atomic read-modify-write | [atomic.cpp](multithreading/atomic.cpp) |
| Wait for one atomic value to change | `atomic::wait` / `notify_one` | [atomic_wait.cpp](multithreading/atomic_wait.cpp) |
| Wait for a compound state predicate | A condition variable and mutex | [condition_variable.cpp](multithreading/condition_variable.cpp) |
| Limit access to a resource pool | `std::counting_semaphore` | [std_semaphore.cpp](multithreading/std_semaphore.cpp) |
| Wait for a one-time batch | `std::latch` | [std_latch.cpp](multithreading/std_latch.cpp) |
| Synchronize repeated phases | `std::barrier` | [std_barrier.cpp](multithreading/std_barrier.cpp) |
| Pass work between producers and consumers | A bounded queue with explicit closure | [producer_consumer.cpp](multithreading/producer_consumer.cpp) |
| Retrieve a result or exception from a few tasks | `std::async(std::launch::async, ...)` and retained futures | [parallel_futures.cpp](asynchrony/parallel_futures.cpp) |
| Reuse a fixed number of workers and retrieve results | A pool with packaged tasks and futures | [thread_pool_futures.cpp](asynchrony/thread_pool_futures.cpp) |

### Lifetime and cancellation

A joinable `std::jthread` requests stop and then joins when destroyed. A stop
request is cooperative: the worker must check its token or use an operation that
supports it. It does not interrupt arbitrary I/O or `sleep_for`. The stop-token
overloads of `condition_variable_any` make an idle wait interruptible. See the
[jthread specification](https://eel.is/c++draft/thread.jthread.class) and
[interruptible waits](https://eel.is/c++draft/thread.condvarany.intwait).

Declare shared state before the threads that use it so the threads are destroyed
first. Join explicitly when a result must be ready before the surrounding scope
ends. For finite work that must finish, `join()` waits without requesting stop;
destroying a stop-aware worker may instead cancel pending work. The queue example
joins its producer before closing the queue, then drains accepted items. The pool
examples drain accepted jobs during shutdown.

Use `std::osyncstream` when several workers print a whole message to the same
stream; a plain chain of `operator<<` calls can interleave with another worker's
message. This protects the output message, not other shared state.

### Algorithms and tasks

`std::execution::par` permits parallel execution; an implementation may still run
sequentially. Keep callbacks independent, avoid shared mutation, and compare
results against a sequential reference. Reduction order may vary, affecting
floating-point rounding. Benchmark representative inputs before claiming a
speedup. See [execution-policy semantics](https://eel.is/c++draft/algorithms.parallel.exec).

`std::async` and futures remain appropriate for a small number of independent
tasks. Retain the futures, specify the launch policy when concurrency matters,
and call `get()` to observe results and exceptions. For many small tasks, use a
pool or a suitable task library to bound worker creation. The hand-written thread pools
here teach ownership and dispatch; their task queues are unbounded. Use the
bounded producer-consumer example to study backpressure.

## Older approaches: compare after the main path

| Comparison example | Why it is kept | Modern starting point |
|---|---|---|
| [legacy/raw_pthread.cpp](multithreading/legacy/raw_pthread.cpp) | POSIX API interoperability and existing platform code | `jthread.cpp` or `parallel_algorithms.cpp` |
| [legacy/is_joinable.cpp](multithreading/legacy/is_joinable.cpp) | Explicit `std::thread` ownership and joinability | `jthread.cpp` |
| [legacy/thread_wrapper.cpp](multithreading/legacy/thread_wrapper.cpp) | A custom join-on-destruction wrapper | `jthread.cpp` |
| [legacy/semaphore.cpp](multithreading/legacy/semaphore.cpp) | How a mutex/condition-variable semaphore works | `std_semaphore.cpp` |
| [legacy/latch.cpp](multithreading/legacy/latch.cpp) | How a countdown wait works | `std_latch.cpp` |
| [legacy/barrier.cpp](multithreading/legacy/barrier.cpp) | How repeated phases can be coordinated | `std_barrier.cpp` |

`std::thread` still has uses where explicit lifetime control is needed. The
comparison directory identifies an alternative learning path; it does not mean
the API has been removed or deprecated. These files use the same C++20 build
command as the rest of this repository; some also use older facilities such as
`std::scoped_lock`, so do not assume every comparison builds as C++11.

## Low-level mechanisms and platform APIs

- [memory_order.cpp](multithreading/memory_order.cpp),
  [spinlock.cpp](multithreading/spinlock.cpp), and
  [merge_sort.cpp](multithreading/merge_sort.cpp) explain memory ordering,
  busy waiting, and recursive partitioning. Start with the higher-level solution
  in the decision table, then use these examples to study implementation details.
- [coroutine_generator.cpp](asynchrony/coroutine_generator.cpp) and
  [coroutine_task.cpp](asynchrony/coroutine_task.cpp) teach C++20 coroutine
  mechanics. Both run synchronously on the calling thread. Coroutines alone
  provide no event loop, executor, or asynchronous I/O. Use an asynchronous I/O
  library such as Boost.Asio when the problem requires event-driven networking.
- The `asynchrony/async_mutex.cpp`, `async_barrier.cpp`, `async_semaphore.cpp`,
  and `async_producer_consumer.cpp` examples coordinate OS threads or
  future-based tasks. Their waits block worker threads; they are not coroutine
  synchronization primitives.

## Processes and IPC

The [`multiprocessing/`](multiprocessing/) examples apply the same ownership rules
to POSIX resources: descriptors, mappings, semaphores, file locks, temporary
paths, and child lifetimes. Shared helpers live in [posix.hpp](common/posix.hpp).
The process pool reuses workers and passes results through pipes; exit codes
report success/failure, not arbitrary computed values. It limits in-flight tasks
to keep IPC buffers bounded.

Start with `posix_spawnp` when launching an existing executable. The
[fork/exec example](multiprocessing/exec_process.cpp) exposes descriptor inheritance
and redirection for comparison. Fork-based C++ child tasks here assume a
single-threaded parent. Signal handling, orphan observation, and `_antipattern`
files are clearly labeled platform studies or intentional failures.

POSIX is the appropriate API for process-shared synchronization in these examples.
An ordinary `std::mutex`, `std::counting_semaphore`, or `std::barrier` does not
promise inter-process behavior. The process mutex and barrier use
`PTHREAD_PROCESS_SHARED`; their backing storage outlives all child participants.
See the [multiprocessing notes](../../notes/03_multiprocessing.md).

## Distributed MPI programs

Every [`mpi/`](mpi/) example uses C++20 ownership around the supported **MPI C
interface**. These C API entry points are current MPI; the removed `MPI::` C++
bindings are not used. [mpi_support.hpp](common/mpi_support.hpp) owns the runtime,
checks errors, and scopes communicators and derived datatypes before finalization.
Unexpected failures abort the job, avoiding a rank quietly exiting while peers
wait in collectives. This is not a general distributed fault-recovery framework.

Choose a collective for the communication pattern. The matrix example accepts
uneven partitions, including ranks with zero rows, and checks a sequential
reference. Nonblocking communication keeps its buffers alive until requests
complete. The single-rank ring case avoids a blocking send to itself. See the
[MPI notes](../../notes/05_mpi.md).

## Asynchronous I/O and future-based work

[parallel_fetch.cpp](asynchrony/parallel_fetch.cpp) uses libcurl multi to progress
at most four HTTP transfers on one thread. Scoped handles, timeouts, per-request
errors, and complete outcome reporting are shared through
[curl_support.hpp](common/curl_support.hpp). The
[download comparison](multithreading/download_images.cpp) uses the same event loop
with concurrency one and four; supply local or remote URLs on the command line.
These network examples do not launch a worker thread per request.

[distributed_computing.cpp](asynchrony/distributed_computing.cpp) is a TCP echo
protocol example with up to 64 owned connections and a 64 KiB output limit per
connection. Nonblocking reads/writes and backpressure keep slow peers from
blocking other connections. It demonstrates socket communication, rather than a
distributed task dispatcher. Client and worker roles retain complete-send and
complete-reply handling.

Futures remain appropriate for threaded computations. Both producer-consumer
examples now share a bounded, stop-aware [queue](common/bounded_queue.hpp) with
explicit closure; consumers do not need a predetermined item count. Coroutine
mechanics remain separate from these actual I/O implementations. See the
[asynchronous-programming notes](../../notes/04_asynchronous_programming.md).

## Build and run

Each `.cpp` file builds as a separate executable with its own `main()`.
The `.hpp` files in [`common/`](common/README.md) are shared headers with no
`main()`; select an example's `.cpp` file when using an IDE's Run action.
Compiling a `.hpp` directly with GCC creates a precompiled header, which cannot
be run as a program. Keep `common/` with the sources so includes resolve.

From the repository root, use a compiler **and standard library** supporting the C++20
facilities used in the selected example:

```bash
g++ -std=c++20 -O2 -Wall -Wextra -pthread src/cpp/multithreading/atomic_wait.cpp -o /tmp/atomic_wait
/tmp/atomic_wait

g++ -std=c++20 -O2 -Wall -Wextra -pthread src/cpp/multithreading/producer_consumer.cpp -o /tmp/producer_consumer
/tmp/producer_consumer
```

For `parallel_algorithms.cpp`, libstdc++ can use a TBB backend. Install the TBB
development package and link with `-ltbb` when that backend is selected:

```bash
g++ -std=c++20 -O2 -pthread src/cpp/multithreading/parallel_algorithms.cpp -o /tmp/parallel_algorithms -ltbb
/tmp/parallel_algorithms
```

A toolchain configured with a serial backend may build without `-ltbb`, but that
run does not demonstrate multi-core execution. See the
[libstdc++ linking requirements](https://gcc.gnu.org/onlinedocs/libstdc++/manual/using.html).

HTTP examples require libcurl headers and `-lcurl`:

```bash
g++ -std=c++20 src/cpp/asynchrony/parallel_fetch.cpp -o /tmp/parallel_fetch -lcurl
/tmp/parallel_fetch https://example.com

g++ -std=c++20 -pthread src/cpp/multiprocessing/spawn_process.cpp -o /tmp/spawn_process
/tmp/spawn_process

mpic++ -std=c++20 src/cpp/mpi/parallel_matrix_multiplication.cpp -o /tmp/mpi_matrix
mpiexec -n 3 /tmp/mpi_matrix
```

POSIX examples require a compatible operating system; MPI examples require
`mpic++` and an MPI runtime.
Antipattern examples may hang, terminate, or invoke undefined behavior; their
headers identify the intentional problem and link to alternatives.

Run the C++ checks for cancellation, queue shutdown, worker reuse, concurrent
IPC runs, MPI partitions, and local HTTP success/failure with:

```bash
python3 -m unittest discover -s tests -p 'test_cpp*.py' -v
```

MPI and HTTP tests skip explicitly if their development dependencies are missing.
Install an MPI C++ compiler wrapper/runtime and libcurl development files to run
all checks. `MPICXX` / `MPIEXEC` can select an MPI toolchain; `CURL_CXXFLAGS` /
`CURL_LDFLAGS` can select a non-default libcurl installation. HTTP tests use a
local server and do not depend on public network services.
