# Multithreading Quiz

Notes: [Multithreading](../notes/02_multithreading.md)

Check your understanding of thread lifecycles, synchronization primitives and thread safety in Python and C++.

#### Q. In Python's `threading` module, which call lets a thread sleep until another thread notifies it?

* [x] `Condition.wait()`
* [ ] `Lock.acquire()`
* [ ] `Thread.join()`
* [ ] `time.sleep()`

> **Explanation:** `Condition.wait()` releases the condition's lock and blocks until `notify()` or `notify_all()` is called. `join()` waits for a thread to finish, not for a notification.

#### Q. Why should a consumer use `condition.wait_for(predicate)` (or re-check its condition in a loop) rather than a single `condition.wait()`?

* [ ] `wait()` returns immediately unless a predicate is passed
* [x] When it wakes up, the shared state may no longer satisfy its condition
* [ ] `wait_for()` keeps the lock held while sleeping, so no other thread can interfere
* [ ] `wait()` can only be woken by `notify_all()`, never by `notify()`

> **Explanation:** Another thread may consume the item first, or the wakeup may be spurious. Checking the predicate while holding the lock after every wakeup keeps the consumer correct.

#### Q. Several Python threads run `counter += 1` on a shared global. How do you make the update reliably thread-safe?

* [ ] Rely on the GIL, since `counter += 1` is a single atomic step
* [x] Wrap the update in `with lock:` using one shared `threading.Lock`
* [ ] Declare `counter` as `global` inside every thread function
* [ ] Set a `threading.Event` before each update

> **Explanation:** `counter += 1` is a read-modify-write, and the GIL is not a synchronization guarantee for it. A shared `Lock` makes the whole update mutually exclusive.

#### Q. Which situation is the best use case for a C++ `std::shared_mutex`?

* [ ] Many threads modify a shared value, but only a few read it
* [ ] Only a few threads both read and modify a shared value
* [x] Many threads read a shared value, but only a few modify it
* [ ] Every thread needs exclusive access to the value

> **Explanation:** A `shared_mutex` lets many readers hold shared ownership at once, while a writer takes exclusive ownership. It pays off when reads greatly outnumber writes.

#### Q. Which statement about C++ `std::shared_mutex` is correct?

* [ ] At most two threads may hold it in shared mode at the same time, one per core
* [x] Many threads may hold it in shared mode, but exclusive mode excludes every other owner
* [ ] A writer can acquire exclusive mode while readers still hold shared mode
* [ ] Several writers may hold exclusive mode together if they modify different fields

> **Explanation:** `lock_shared()` allows concurrent readers; `lock()` waits until no reader or writer holds the mutex. While one thread has exclusive ownership, nobody else can hold it in either mode.

#### Q. Why would you mark a Python background thread as a daemon thread?

* [ ] Daemon threads always run at the lowest scheduling priority
* [ ] Daemon threads are restarted automatically by the interpreter if they crash
* [x] A daemon thread does not keep the program alive when the main thread finishes
* [ ] Daemon threads run in a separate process, so they bypass the GIL

> **Explanation:** The interpreter waits for non-daemon threads at shutdown but not for daemon threads. Daemons suit best-effort work, such as a heartbeat, that can be abandoned at exit.

#### Q. Why is it risky to write important log records from a daemon thread?

* [ ] Daemon threads are not allowed to open files for writing
* [ ] Every record a daemon writes is duplicated in the log
* [x] It may be stopped abruptly at exit, losing the last writes
* [ ] Daemon threads run without the process's file permissions

> **Explanation:** Daemon threads are stopped without cleanup when the interpreter exits, so buffers may never be flushed. Important work should use a non-daemon thread with a stop signal and `join()`.

#### Q. In C++, what happens right after a `std::thread` object is constructed with a callable?

* [x] It starts running the callable, subject to OS scheduling
* [ ] The thread waits until the parent calls `start()`
* [ ] The thread waits until the parent calls `join()`
* [ ] The thread stays idle until `detach()` is called

> **Explanation:** `std::thread` has no `start()` method; construction launches the thread. `join()` and `detach()` only decide how its lifetime is managed.

#### Q. What happens if a `std::thread` object is destroyed while it is still joinable?

* [ ] The thread is detached automatically
* [ ] The destructor joins the thread automatically
* [x] The program calls `std::terminate`
* [ ] The thread is cancelled at its next blocking call

> **Explanation:** Destroying a joinable `std::thread` is treated as an error, so the program terminates. Every `std::thread` must be joined or detached first, or use `std::jthread`.

#### Q. What does `std::jthread` (C++20) do in its destructor if the thread is still running?

* [ ] It detaches the thread and returns immediately without waiting
* [x] It requests a stop through its stop token and then joins the thread
* [ ] It calls `std::terminate`, exactly like `std::thread`
* [ ] It forcibly kills the thread without waiting for it to finish

> **Explanation:** `std::jthread` calls `request_stop()` and then `join()`. The thread function can watch its `std::stop_token` to exit promptly, which makes RAII-style cleanup safe.

#### Q. In C++, after calling `t.detach()`, what does `t.joinable()` return?

* [x] `false`
* [ ] `true`
* [ ] `true` until the detached thread finishes
* [ ] It throws `std::system_error`

> **Explanation:** Detaching disassociates the thread from the `std::thread` object, so the object no longer represents a thread and cannot be joined.

#### Q. What does a barrier guarantee for a group of threads?

* [ ] Only one thread at a time can pass the barrier
* [x] No thread passes it until all participants have arrived
* [ ] Operations before the barrier execute as one atomic step
* [ ] Threads resume in the order in which they arrived

> **Explanation:** A barrier is a meeting point. Each thread waits there until the required number of threads arrive, which controls the relative order of the phases before and after it.

#### Q. In Python, a thread must finish a section of code before other threads pass a `threading.Barrier`. It should call the barrier's _____ method _____ executing that code.

* [ ] `wait()`; before
* [x] `wait()`; after
* [ ] `notify_all()`; after
* [ ] `release()`; before

> **Explanation:** Calling `wait()` after the code means the thread reaches the meeting point only once its work is done. No participant passes the barrier until all have called `wait()`.

#### Q. Workers must all finish phase k before any of them starts phase k+1, and this repeats 100 times. Which C++20 primitive fits best?

* [x] `std::barrier`, calling `arrive_and_wait()` after each phase
* [ ] `std::latch`, with each worker calling `count_down()` per phase
* [ ] `std::counting_semaphore`, acquired at the start of each phase
* [ ] `std::recursive_mutex`, locked by each worker around each phase

> **Explanation:** A `std::barrier` resets automatically after each phase, so it suits repeated phases. A `std::latch` can be used only once.

#### Q. In C++, a thread has locked a `std::recursive_mutex` three times. How many times must it unlock it before another thread can acquire it?

* [ ] Once
* [ ] Twice
* [x] Three times, once per lock
* [ ] None, because other threads can share it

> **Explanation:** A recursive mutex keeps an ownership count for the owning thread. It becomes available to other threads only when the count returns to zero.

#### Q. A function holds a `threading.Lock` and calls a helper that acquires the same lock in the same thread. What happens?

* [ ] The helper acquires the lock again because the same thread owns it
* [x] The thread blocks forever, because `Lock` is not reentrant
* [ ] Python raises `RuntimeError` immediately
* [ ] The first acquisition is released automatically

> **Explanation:** A plain `Lock` has no owner tracking, so a second `acquire()` waits forever. An `RLock` lets its owning thread acquire it repeatedly, as long as it releases it the same number of times.

#### Q. In C++, what does `try_lock()` do when the mutex is already locked by another thread?

* [ ] Blocks until the mutex becomes available, then returns `true`
* [x] Returns `false` immediately without blocking
* [ ] Keeps retrying in a loop until it succeeds
* [ ] Throws an exception describing the owner

> **Explanation:** `try_lock()` never waits. Unlike `lock()`, it reports failure so the caller can decide what to do instead of blocking.

#### Q. Why is `try_lock()` useful?

* [x] It lets a thread do other work if the mutex it needs is already taken
* [ ] It guarantees fair, first-come-first-served access to the mutex
* [ ] It picks a random winner when several threads lock simultaneously
* [ ] It detects deadlocks and breaks them automatically

> **Explanation:** A thread can attempt the lock, and if it is busy, process other items or back off and retry later. It provides no fairness guarantee.

#### Q. Why would ThreadA call `join()` on ThreadB?

* [ ] So that ThreadB waits until ThreadA has terminated
* [x] So that ThreadA waits until ThreadB has terminated
* [ ] To wake ThreadB up if it is blocked
* [ ] To terminate ThreadB immediately

> **Explanation:** `join()` blocks the calling thread until the target thread finishes, for example before ThreadA uses ThreadB's results. It never stops or wakes the target.

#### Q. When would you call a condition variable's `notify_one()` instead of `notify_all()`?

* [ ] When a specific, named waiting thread must wake up
* [ ] When every waiting thread must re-check the condition
* [x] When any single waiter can handle the change
* [ ] When no thread is currently waiting on it

> **Explanation:** `notify_one()` wakes a single, unspecified waiter, for example when one item has been added to a queue. Waking everyone would make the rest compete and go back to sleep.

#### Q. What should a thread do before notifying a condition variable?

* [ ] Notify first, then lock the mutex and update the state
* [x] Update the shared state while holding the associated mutex
* [ ] Wait on the same condition variable to check that someone is listening
* [ ] Update the state without the mutex, because notification provides the ordering

> **Explanation:** Waiters check a predicate under the mutex, so the state change must also be made under it. The notification can be sent before or after unlocking; changing state unlocked risks missed wakeups.

#### Q. Which is a common use case for a counting semaphore?

* [x] Limiting concurrent use of a pool of identical resources
* [ ] Counting how many threads the program has created
* [ ] Measuring how long a program has been running
* [ ] Recording the order in which threads started

> **Explanation:** The count represents available resources, such as database connections. Acquiring takes one slot, releasing returns it, and threads wait when none are free.

#### Q. A `threading.Semaphore(3)` guards code that appends to a shared Python list. Is the list protected from concurrent modification?

* [ ] Yes, a semaphore always provides mutual exclusion
* [x] No, up to three threads can be inside at once, so the list needs its own lock
* [ ] Yes, because a semaphore count above one makes it a reader-writer lock
* [ ] No, because semaphores can only be used between processes

> **Explanation:** A count above one deliberately lets several threads in at the same time. It limits concurrency but does not make access to a shared mutable object exclusive.

#### Q. Which two mechanisms are commonly combined to build a thread-safe blocking queue in C++?

* [ ] A semaphore and a thread-local variable
* [ ] An atomic counter and a spinlock
* [x] A mutex and a condition variable
* [ ] Nothing, since `std::queue` is already thread-safe

> **Explanation:** The mutex protects the queue's contents, and the condition variable lets consumers sleep until an item arrives. `std::queue` itself provides no thread safety.

#### Q. In Python, which ready-made class provides a thread-safe FIFO for producer-consumer code?

* [x] `queue.Queue`
* [ ] `collections.OrderedDict`
* [ ] `threading.local`
* [ ] `threading.Barrier`

> **Explanation:** `queue.Queue` handles its own locking, and `get()` can block until an item is available. With `maxsize`, `put()` also blocks when it is full, giving backpressure.

#### Q. What does a semaphore's `acquire()` operation do to its count?

* [ ] Always decrements it, even below zero
* [x] Waits until the count is positive, then decrements it
* [ ] Increments it if the count is currently positive
* [ ] Resets it to the value it was created with

> **Explanation:** If the count is zero, `acquire()` blocks the caller. Once a slot is available, the count is decremented atomically and the caller proceeds.

#### Q. What does a semaphore's `release()` operation do?

* [ ] Decrements the count and blocks if it reaches zero
* [x] Increments the count and may wake a thread waiting to acquire
* [ ] Blocks every thread that is waiting on the semaphore
* [ ] Resets the count to zero

> **Explanation:** Releasing returns a slot. If a thread is blocked in `acquire()`, it can now take that slot and continue.

#### Q. Which statement describes a difference between a binary semaphore and a mutex?

* [ ] A binary semaphore can count to any positive value
* [x] A binary semaphore can be acquired by one thread and released by another
* [ ] A binary semaphore must be released by the thread that acquired it
* [ ] A binary semaphore can hold negative values

> **Explanation:** A mutex has an owner that must unlock it. A binary semaphore has no ownership, which makes it suitable for signalling between threads.

#### Q. What is a monitor in concurrent programming?

* [ ] A debugging tool that records the activity of every thread
* [x] A construct that combines shared state, a mutex and condition variables
* [ ] A background thread that watches other threads for crashes
* [ ] An OS service that reports how much CPU each thread uses

> **Explanation:** A monitor lets only one thread at a time run inside it and provides condition variables for waiting on state changes. A bounded buffer with `not_full` and `not_empty` conditions is a classic example.

#### Q. What does `executor.submit(fn, x)` return for a `concurrent.futures.ThreadPoolExecutor`?

* [ ] The value of `fn(x)`, after waiting for it
* [x] A `Future` representing the pending call
* [ ] The `Thread` object that will run `fn`
* [ ] `None`, because results are discarded

> **Explanation:** `submit()` schedules the call on a worker thread and returns immediately. Call `future.result()` to wait for the value or re-raise the exception from `fn`.

#### Q. Why are thread pools useful?

* [ ] Threads in a pool can share data more easily than other threads
* [ ] They automatically prevent data races between tasks
* [x] They reuse threads, avoiding the cost of creating a thread for every task
* [ ] They guarantee that tasks run in the order they were submitted

> **Explanation:** A pool keeps a bounded set of worker threads that pull tasks from a queue. This amortises creation cost and limits how many threads run at once.

#### Q. What does `threading.Timer(5.0, f).start()` do?

* [ ] Calls `f` every 5 seconds until the timer is cancelled
* [x] Calls `f` once, in a new thread, after about 5 seconds
* [ ] Blocks the calling thread for 5 seconds and then calls `f`
* [ ] Runs `f` and raises an error if it takes longer than 5 seconds

> **Explanation:** A `Timer` is a thread that waits and then runs its function once. `cancel()` stops it if called before the delay ends; periodic work needs a loop.

#### Q. What does `Thread.is_alive()` return?

* [ ] The thread's numeric identifier, or `None`
* [x] Whether it has started and not yet finished
* [ ] The thread's exit code, once it has finished
* [ ] Whether the thread was created as a daemon

> **Explanation:** `is_alive()` is `True` from just before `run()` starts until just after it returns. It is often used after a `join()` with a timeout to see whether the thread finished.

#### Q. Which `threading` object lets one thread set a flag that releases every thread blocked in its `wait()` and stays set for later callers?

* [ ] `threading.Lock`
* [ ] `threading.Barrier`
* [x] `threading.Event`
* [ ] `threading.Timer`

> **Explanation:** `Event.set()` wakes all waiters, and later `wait()` calls return immediately until `clear()` is called. A barrier releases threads only once enough of them arrive.

#### Q. What is the difference between calling `t.start()` and calling `t.run()` on a `threading.Thread`?

* [ ] They are equivalent, because `start()` is simply an alias for `run()`
* [x] `start()` runs the target in a new thread; `run()` runs it in the calling thread
* [ ] `run()` creates the OS thread; `start()` only schedules it to run
* [ ] `start()` waits for the target to finish; `run()` returns immediately

> **Explanation:** `start()` creates the OS thread, which then calls `run()`. Calling `run()` directly is just an ordinary function call, so nothing runs concurrently.

#### Q. What is the recommended way to stop a long-running Python worker thread?

* [ ] Call `thread.stop()` and then `join()` it
* [ ] Call `thread.terminate()` to end it at once
* [ ] Call `thread.kill(signal.SIGTERM)` on it
* [x] Signal it with an `Event` it checks, then `join()` it

> **Explanation:** Python threads have no stop, terminate or kill method. Cooperative stopping lets the worker finish its current step and clean up before it exits.

#### Q. What happens if you call `start()` twice on the same `threading.Thread` object?

* [ ] The thread restarts from the beginning
* [ ] The second call is silently ignored
* [x] A `RuntimeError` is raised
* [ ] The second call blocks until the first run completes

> **Explanation:** A thread object can be started only once. To run the work again, create a new `Thread`.

#### Q. What is a spinlock, and when is it appropriate?

* [ ] A lock that keeps a thread asleep until the OS wakes it, for long waits
* [x] A lock that busy-waits until it is free, for very short critical sections
* [ ] A lock that rotates ownership between threads in a fixed order
* [ ] A lock used only to protect disk I/O

> **Explanation:** Spinning avoids the cost of sleeping and waking when the lock will be released almost immediately. For longer waits it wastes CPU time that other threads could use.

#### Q. What is a livelock?

* [ ] A thread that is permanently blocked waiting on one lock
* [ ] Another name for a deadlock between two threads
* [x] Threads that keep reacting to each other but make no progress
* [ ] A lock that keeps a thread alive after the program exits

> **Explanation:** Livelocked threads are active, for example repeatedly backing off and retrying in step, but no work gets done. In a deadlock the threads are blocked instead.

#### Q. What is thread-local storage, such as Python's `threading.local()`?

* [ ] Memory that only the main thread may access
* [x] Data for which each thread sees its own independent copy
* [ ] A cache located physically close to the CPU core
* [ ] Encrypted storage for sensitive thread data

> **Explanation:** Each thread that sets an attribute on a `threading.local()` object gets its own value, so no locking is needed for that data.

#### Q. What is lock-free programming?

* [ ] Sharing data between threads with no synchronization at all
* [x] Using atomics instead of locks so some thread always progresses
* [ ] Letting the compiler insert the necessary locks automatically
* [ ] Restricting a program to one thread so no locks are needed

> **Explanation:** Lock-free algorithms use atomics such as compare-and-swap instead of mutexes, so a stalled thread cannot block others. They are hard to get right and can suffer from problems like ABA.

#### Q. What does a compare-and-swap (CAS) operation do?

* [ ] Swaps the values of two variables and returns the larger
* [x] Atomically writes a new value only if the old one is as expected
* [ ] Compares two memory regions and reports the first difference
* [ ] Exchanges the stacks of two threads during a context switch

> **Explanation:** CAS reports whether it succeeded, so a thread can retry with a fresh value if another thread changed the location first. It is the building block of many lock-free algorithms.

#### Q. What is the thundering herd problem?

* [ ] Too many threads being created at once when the program starts
* [x] Many waiting threads being woken at once when only one can proceed
* [ ] Threads that each keep spawning more and more child threads
* [ ] Memory exhaustion caused by large thread stacks

> **Explanation:** After a broadcast wakeup, all the threads compete for the resource and most go back to sleep, wasting CPU time and causing contention. Waking only one waiter avoids it.

#### Q. Why do Python threads rarely speed up CPU-bound pure-Python code in a standard (GIL-enabled) CPython build?

* [x] The GIL lets only one thread execute Python bytecode at a time
* [ ] Python threads are always scheduled on the same core as the main thread
* [ ] Each Python thread is limited to a fixed share of CPU time
* [ ] CPython runs threads one after another, never interleaving them

> **Explanation:** CPU-bound threads take turns holding the GIL, so they gain little or no speed. Threads still help I/O-bound work, and processes or native code are used for CPU-bound parallelism.

#### Q. What problem does this C++ code have when several threads run it on a shared `std::atomic<int> counter`?

```cpp
counter.store(counter.load() + 1);
```

* [ ] None, because both calls are atomic
* [x] Updates can be lost, because another thread can change `counter` between the load and the store
* [ ] It is undefined behavior, because atomics cannot be loaded and stored in one statement
* [ ] It deadlocks, because `load()` locks the atomic until `store()` is called

> **Explanation:** Each call is atomic, but the read-modify-write sequence is not. `counter.fetch_add(1)` performs the whole increment as one atomic operation.

#### Q. A producer fills a buffer and then stores `ready = true` with `memory_order_release`. Which operation must the consumer perform on `ready` so it is guaranteed to see the buffer's contents?

* [ ] A load with `memory_order_relaxed` that reads `true`
* [x] A load with `memory_order_acquire` that reads `true`
* [ ] A plain non-atomic read of `ready` in a loop
* [ ] A store with `memory_order_acquire` to `ready`

> **Explanation:** A release store paired with an acquire load that reads its value makes all writes before the store visible after the load. Relaxed orders only the flag itself, and an acquire store is not valid.

#### Q. Two threads run these functions concurrently. Which change prevents the possible deadlock?

```python
def task1():
    with lock1:
        with lock2:
            work()

def task2():
    with lock2:
        with lock1:
            work()
```

* [ ] Add `time.sleep()` between the two `with` statements
* [x] Make both functions acquire `lock1` before `lock2`
* [ ] Replace both locks with `threading.Event` objects
* [ ] Start `task2` before `task1`

> **Explanation:** Opposite acquisition orders can form a cycle where each thread holds one lock and waits for the other. A single global lock order breaks the circular wait.

#### Q. In C++, what is the consequence of a data race on a plain (non-atomic) `int`?

* [ ] The final value is always one of the values that were written
* [ ] The program throws `std::system_error` when the race occurs
* [x] The behavior of the whole program is undefined
* [ ] The compiler rejects the program at compile time

> **Explanation:** The C++ memory model makes data races undefined behavior, so even a seemingly correct result proves nothing. Use a mutex or `std::atomic` for shared data.

#### Q. Thread A updates `balance` while holding `mutex_m`. Thread B reads `balance` without locking anything. Is B's read safe?

* [ ] Yes, because only writes need to hold the mutex
* [ ] Yes, because thread A holds the mutex during its write
* [x] No, every conflicting access to `balance` must use the same mutex
* [ ] No, because reads are never allowed while a mutex exists

> **Explanation:** A lock protects only the code that uses it. B's unsynchronized read races with A's write, so B could see a stale or torn value.

#### Q. A CPU-bound C++ program runs well with 8 threads on an 8-core machine. What usually happens if it uses 200 threads instead?

* [ ] It becomes about 25 times faster, one thread per unit of work
* [ ] It runs at the same speed, since extra threads simply wait
* [x] It gains nothing and may slow down from context switching and contention
* [ ] It fails, because the OS allows only one thread per CPU core

> **Explanation:** Once every core is busy, extra CPU-bound threads only add scheduling overhead, cache thrashing and contention. More threads do not mean more performance.

#### Q. Why prefer `std::lock_guard` or `std::scoped_lock` over calling `lock()` and `unlock()` by hand?

* [ ] They make the mutex faster to acquire under contention
* [x] They unlock automatically on scope exit, even on exceptions
* [ ] They allow several threads to own the mutex at the same time
* [ ] They turn the mutex into a recursive mutex automatically

> **Explanation:** RAII guards unlock in their destructor, so an early return or exception cannot leave the mutex locked. `std::scoped_lock` can also lock several mutexes at once without deadlock.
