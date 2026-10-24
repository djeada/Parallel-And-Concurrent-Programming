# Asynchronous Programming Quiz

Notes: [Asynchronous Programming](../notes/04_asynchronous_programming.md)

Check your understanding of coroutines, event loops, futures and tasks in Python's `asyncio` and C++.

#### Q. What type of object does C++ `std::async()` return?

* [x] `std::future`
* [ ] `std::thread`
* [ ] `std::promise`
* [ ] `bool`

> **Explanation:** `std::async` runs a callable and returns a `std::future` that will hold its result or exception. Call `get()` on the future to retrieve it.

#### Q. What is the purpose of a future?

* [ ] It schedules a task to run later on a thread pool
* [x] It is a placeholder for a result that may not have been computed yet
* [ ] It records how a program should behave on its next run
* [ ] It retries an operation automatically whenever it fails

> **Explanation:** A future starts pending and later completes with a value or an error. The work itself is done elsewhere, for example by a thread or an event loop.

#### Q. A function launched with `std::async` throws an exception. What happens when the caller calls `get()` on the returned future?

* [ ] `get()` returns a default-constructed value
* [x] The exception is re-thrown in the calling thread
* [ ] The program calls `std::terminate` immediately
* [ ] `get()` blocks forever because no value was set

> **Explanation:** The future stores the exception in its shared state, and `get()` re-throws it, so the caller can handle it with an ordinary `try`/`catch`.

#### Q. Several C++ threads all need to read the result of one asynchronous computation. What should they use?

* [ ] One `std::future`, calling `get()` from every thread
* [x] A `std::shared_future`, for example from `future.share()`
* [ ] A separate `std::promise` per thread, all set from the same future
* [ ] A `std::mutex` that stores the result

> **Explanation:** A `std::future` can be retrieved with `get()` only once. A `std::shared_future` can be copied, and every copy may call `get()` on the same result.

#### Q. What role does a `std::promise` play?

* [ ] It launches a callable on a new thread and returns its result
* [x] It lets a producer set the value its future will deliver
* [ ] It cancels a pending future and releases its shared state
* [ ] It blocks until a given future becomes ready

> **Explanation:** The producer calls `set_value()` or `set_exception()` on the promise. The consumer receives the result through the `std::future` obtained from `get_future()`.

#### Q. When does a function launched with `std::async(std::launch::deferred, f)` run?

* [ ] Immediately, on a newly created thread
* [x] Lazily, in the thread that first calls `get()` or `wait()`
* [ ] Only when the future is destroyed at program exit
* [ ] Later, on a worker thread from a global thread pool

> **Explanation:** Deferred launch creates no thread; the call happens on demand in the waiting thread. Use `std::launch::async` to require asynchronous execution.

#### Q. How do you chain a second step after a `std::future` in standard C++20?

* [ ] Call `future.then(next_step)`
* [x] Call `get()` and pass the result to the next step yourself
* [ ] Pass the future to `std::async` with `std::launch::chain`
* [ ] Assign `next_step` to `future.on_ready`

> **Explanation:** `std::future` has no `.then()` in standard C++; that API stayed in a Technical Specification. Libraries such as Folly, HPX and Boost provide continuations as extensions.

#### Q. How does a coroutine differ from a regular function?

* [ ] A coroutine always runs on its own dedicated thread
* [ ] A coroutine cannot return a value to its caller
* [x] A coroutine can suspend and later resume where it left off
* [ ] A coroutine always runs faster than a function

> **Explanation:** A regular function runs until it returns. A coroutine can pause, for example at an `await`, letting a scheduler run other work before resuming it where it left off.

#### Q. In Python, what happens when you call an `async def` function without awaiting it or turning it into a task?

* [ ] Its body runs synchronously and its result is discarded
* [ ] Its body runs in a background thread automatically
* [x] Only a coroutine object is created; the body never runs
* [ ] A `SyntaxError` is raised at the call site

> **Explanation:** Calling an `async def` function only creates a coroutine object. Its body runs when it is awaited or scheduled with `asyncio.create_task()`; otherwise a `RuntimeWarning` appears when it is garbage-collected.

#### Q. How does calling a JavaScript `async` function differ from calling a Python `async def` function?

* [ ] JavaScript runs the function on a worker thread; Python runs it on the event loop
* [x] JavaScript runs the body up to its first `await`; Python runs none of it yet
* [ ] JavaScript returns the result directly; Python returns a future
* [ ] There is no difference; both start running when awaited

> **Explanation:** A JavaScript async function runs synchronously until its first `await` and returns a promise. A Python coroutine does nothing until it is awaited or scheduled.

#### Q. In Python, what does `await` do inside a coroutine?

* [ ] Starts a new operating-system thread to run the awaited call
* [ ] Blocks the whole thread, and so every task, until it completes
* [x] Suspends the coroutine, letting other tasks run until the result is ready
* [ ] Cancels the awaited operation if it is not already complete

> **Explanation:** `await` waits for a result without blocking the thread. If the awaitable is not ready, control returns to the event loop, which resumes the coroutine later.

#### Q. What is an event loop?

* [ ] A loop that polls the keyboard and mouse for user input events
* [x] A scheduler that resumes tasks and callbacks as I/O and timers complete
* [ ] A debugging tool that traces the execution of asynchronous code
* [ ] A thread pool that runs each coroutine on its own thread

> **Explanation:** The event loop tracks pending I/O, timers and ready work. It can manage many concurrent operations on a single thread because tasks take turns as they become ready.

#### Q. Which `asyncio` function wraps a coroutine in a Task so it starts running concurrently on the event loop?

* [ ] `asyncio.start_task()`
* [ ] `asyncio.new_task()`
* [x] `asyncio.create_task()`
* [ ] `asyncio.schedule()`

> **Explanation:** `create_task()` schedules the coroutine and returns a `Task`, a kind of future you can await, cancel or inspect. It must be called while an event loop is running.

#### Q. How does `asyncio.run(main())` differ from `loop.run_until_complete(main())`?

* [x] `asyncio.run()` creates, runs and closes a new loop; the other uses an existing loop
* [ ] `asyncio.run()` runs the coroutine in a background thread; `run_until_complete()` uses the main thread
* [ ] `asyncio.run()` returns immediately; `run_until_complete()` waits for the result
* [ ] They are identical aliases kept for backward compatibility

> **Explanation:** `asyncio.run()` is the high-level entry point that manages the loop's whole lifetime, including cleanup. `run_until_complete()` is a lower-level method on a loop you manage yourself.

#### Q. What does this code print?

```python
import asyncio

async def fetch(n):
    await asyncio.sleep(n / 10)
    return n

async def main():
    print(await asyncio.gather(fetch(3), fetch(1), fetch(2)))

asyncio.run(main())
```

* [x] `[3, 1, 2]`
* [ ] `[1, 2, 3]`
* [ ] `[3, 2, 1]`
* [ ] The order varies from run to run

> **Explanation:** The coroutines run concurrently and finish in the order 1, 2, 3, but `gather()` returns results in the order the awaitables were passed in.

#### Q. What does `asyncio.gather(..., return_exceptions=True)` change?

* [ ] It cancels the remaining tasks as soon as one fails
* [x] Exceptions appear in the result list instead of being raised
* [ ] Failed awaitables are retried automatically once
* [ ] Exceptions are written to the log and then discarded

> **Explanation:** Without it, the first exception is raised to the awaiting code. With it, every awaitable finishes and failures appear in their positions in the result list.

#### Q. One task inside an `asyncio.TaskGroup` (Python 3.11+) raises `ValueError`. What happens to the other tasks in the group?

* [ ] They keep running, and the error is raised once they finish
* [x] They are cancelled, then the error is raised in an `ExceptionGroup`
* [ ] They are restarted from the beginning after a delay
* [ ] They are paused until the failed task is retried

> **Explanation:** A `TaskGroup` treats related tasks as one unit: a failure cancels the remaining tasks, waits for them to finish, then raises the collected errors.

#### Q. How does `asyncio.wait()` differ from `asyncio.gather()`?

* [ ] `wait()` accepts coroutines only; `gather()` accepts only threads
* [x] `wait()` returns `(done, pending)` sets; `gather()` returns ordered results
* [ ] `wait()` blocks the event loop thread; `gather()` does not
* [ ] `wait()` always returns immediately; `gather()` waits for every task

> **Explanation:** `wait()` lets you react when the first task completes or the first one fails, via `return_when`, but you read results from the done tasks yourself. `gather()` waits for all and returns an ordered list.

#### Q. Roughly how long does this program take to run?

```python
import asyncio

async def work():
    await asyncio.sleep(1)

async def main():
    await work()
    await work()

asyncio.run(main())
```

* [ ] About 0 seconds
* [ ] About 1 second
* [x] About 2 seconds
* [ ] About 4 seconds

> **Explanation:** Each `await` waits for one `work()` to finish before starting the next, so the sleeps happen sequentially. `asyncio.gather(work(), work())` would overlap them and take about 1 second.

#### Q. How does `await asyncio.sleep(1)` differ from `time.sleep(1)` inside a coroutine?

* [ ] They behave identically inside a coroutine
* [x] `asyncio.sleep()` suspends one task; `time.sleep()` blocks the loop
* [ ] `asyncio.sleep()` runs on a separate thread; `time.sleep()` runs on the event loop
* [ ] `time.sleep()` is asynchronous; `asyncio.sleep()` is not

> **Explanation:** `time.sleep()` keeps the thread busy, so no other task can run during it. `asyncio.sleep()` hands control back to the loop until the timer expires.

#### Q. A coroutine must call a library function that performs blocking I/O. How can you keep the event loop responsive?

* [ ] Wrap the library function in an `async def`
* [x] Offload it with `await asyncio.to_thread(func)`
* [ ] Call it inside `async with` so it becomes nonblocking
* [ ] Call `asyncio.sleep(0)` just before the blocking call

> **Explanation:** Declaring a function `async` does not make its synchronous body nonblocking. Offloading it to a thread, with `to_thread()` or `run_in_executor()`, lets the loop keep running other tasks while the call waits.

#### Q. Where can the `asyncio` event loop switch from one running coroutine to another?

* [ ] At any bytecode instruction, like OS thread preemption
* [x] Only where the running coroutine suspends, such as at an `await`
* [ ] Only when a coroutine returns or raises an exception
* [ ] Every 10 milliseconds, when a timer interrupt fires

> **Explanation:** asyncio is cooperative. A coroutine that computes for a long time without awaiting keeps every other task waiting until it suspends or finishes.

#### Q. What is "callback hell", and how does `async`/`await` help?

* [ ] Callbacks that run too fast to debug; `await` slows them down
* [x] Deeply nested callbacks; `await` lets the logic be written linearly
* [ ] Callbacks that never execute; `await` forces them to run
* [ ] Memory leaks caused by callbacks; `await` frees the memory

> **Explanation:** Each dependent step used to be nested inside the previous step's callback. With `await`, sequential steps and `try`/`except` error handling read like ordinary synchronous code.

#### Q. A producer coroutine calls `await queue.put(item)` on a full `asyncio.Queue(maxsize=100)`. What happens?

* [ ] The item is dropped silently to protect memory
* [ ] `queue.put()` raises `asyncio.QueueFull` immediately
* [x] The producer waits until a consumer removes an item
* [ ] The queue grows beyond 100 items to make room

> **Explanation:** A bounded queue applies backpressure: fast producers are slowed to the consumers' pace instead of letting memory grow without limit. `put_nowait()` is the variant that raises `QueueFull`.

#### Q. What is `async with` used for?

* [ ] Declaring variables that are shared between coroutines
* [x] Managing resources whose setup and cleanup must be awaited
* [ ] Converting a synchronous block into asynchronous code
* [ ] Running the body in parallel on several threads

> **Explanation:** An async context manager awaits its `__aenter__` and `__aexit__` methods, so connections or files can be opened and closed without blocking the loop.

#### Q. What is `async for` used for?

* [ ] Running each loop iteration on a separate thread
* [x] Iterating where fetching each next item may need an `await`
* [ ] Iterating over a list of futures in completion order
* [ ] Making an ordinary `for` loop run faster on the event loop

> **Explanation:** `async for` calls `__anext__()` and awaits it for each item, so the loop can wait for data, such as rows from a stream, without blocking the event loop.

#### Q. What is an async generator in Python?

* [ ] A tool that converts synchronous code into coroutines
* [x] An `async def` function that contains `yield`
* [ ] A class that creates a new event loop for each value
* [ ] A generator that runs on a background thread

> **Explanation:** An async generator is consumed with `async for` and can `await` between yields, which makes it a convenient way to stream values that arrive over time.

#### Q. A coroutine sends requests to an API that allows at most 10 concurrent calls. Which `asyncio` primitive enforces the limit?

* [ ] `asyncio.Event`, set after every 10 requests
* [x] `asyncio.Semaphore(10)` around each request
* [ ] `asyncio.Lock`, held for the whole batch
* [ ] `asyncio.Barrier(10)` before each batch

> **Explanation:** Each request acquires the semaphore before calling the API. When 10 are in flight, further requests wait until one finishes and releases its slot.

#### Q. Single-threaded asyncio code updates shared state. When does it still need an `asyncio.Lock`?

* [ ] Never, because only one coroutine runs at a time
* [x] When the update contains an `await` partway through
* [ ] Whenever two coroutines read the same variable
* [ ] Only when the program uses more than one CPU core

> **Explanation:** Without an `await` inside the update, no other task can interleave. If the critical section suspends, another coroutine may see or change half-updated state.

#### Q. What is `asyncio.Condition` used for?

* [ ] Checking whether the event loop is currently running
* [x] Waiting until shared state meets a condition, then being notified
* [ ] Setting preconditions that a task must meet before it is created
* [ ] Testing network conditions before sending requests

> **Explanation:** It combines an `asyncio.Lock` with notification, mirroring `threading.Condition`. Coroutines `await cond.wait_for(predicate)` and producers call `notify()` after changing the state.

#### Q. An exception is raised inside a coroutine. Where does it surface?

* [ ] Nowhere, because exceptions in coroutines are ignored
* [ ] It always crashes the whole program immediately
* [x] In the code that awaits the coroutine or its task
* [ ] Only in the event loop's debug log

> **Explanation:** Awaiting re-raises the exception, so ordinary `try`/`except` works. A task whose exception is never retrieved produces a warning instead.

#### Q. What happens when `task.cancel()` is called on a running `asyncio` task?

* [ ] The task is stopped instantly, even in the middle of a statement
* [x] `CancelledError` is raised inside it at its current `await`
* [ ] The task finishes normally but its result is discarded
* [ ] The event loop is closed along with the task

> **Explanation:** Cancellation is cooperative: the coroutine sees `CancelledError` at its current or next `await`. It should release resources in `finally` and normally let the error propagate.

#### Q. In Python 3.11 and later, what exception does `asyncio.wait_for()` raise when the timeout expires?

* [x] The built-in `TimeoutError`
* [ ] `asyncio.CancelledError`
* [ ] `concurrent.futures.CancelledError`
* [ ] `asyncio.InvalidStateError`

> **Explanation:** Since 3.11, `asyncio.TimeoutError` and `concurrent.futures.TimeoutError` are both aliases of the built-in `TimeoutError`. The awaited task is cancelled internally when the timeout expires.

#### Q. What does `asyncio.timeout(5)` (Python 3.11+) provide?

* [ ] A global timeout applied to every coroutine in the program
* [x] An async context manager that cancels its block after 5 seconds
* [ ] A decorator that retries a coroutine for up to 5 seconds
* [ ] A 5-second pause before the block starts running

> **Explanation:** `async with asyncio.timeout(5):` cancels the work inside if the deadline passes and raises `TimeoutError` at the end of the block.

#### Q. What does `await asyncio.shield(task)` protect against?

* [ ] Exceptions raised inside the shielded task
* [x] Outer cancellation propagating into the inner task
* [ ] Timeouts raised by the task's own code
* [ ] Other tasks reading or changing the task's state

> **Explanation:** If the outer coroutine is cancelled, the shielded task keeps running, while the outer `await` still raises `CancelledError`.

#### Q. What is `asyncio.StreamReader` used for?

* [ ] Reading configuration files once at start-up
* [x] Asynchronously reading from a stream such as a TCP socket
* [ ] Reading from standard input synchronously, line by line
* [ ] Decoding compressed video streams on a GPU

> **Explanation:** `asyncio.open_connection()` returns a `StreamReader` and `StreamWriter`. Methods such as `await reader.readline()` wait for data without blocking the event loop.

#### Q. What does `future.cancel()` return for a `concurrent.futures.Future` whose call is already running?

* [ ] `True`, and the worker thread is stopped
* [x] `False`, because a running call cannot be cancelled
* [ ] `None`, after waiting for the call to finish
* [ ] It raises `CancelledError` in the caller

> **Explanation:** Only calls still waiting in the executor's queue can be cancelled. A running or finished call returns `False`, and its result can still be retrieved with `result()`.

#### Q. How do you enable asyncio's debug mode to detect slow callbacks and never-awaited coroutines?

* [ ] Call `asyncio.debug()` at start-up
* [x] Pass `debug=True` to `asyncio.run()`
* [ ] Import the `asyncio.debugger` module
* [ ] Pass `--asyncio-debug` to `python`

> **Explanation:** There is no `asyncio.debug()` function. Debug mode can also be enabled with `PYTHONASYNCIODEBUG=1`, `loop.set_debug(True)` or development mode (`-X dev`).

#### Q. A server holds thousands of mostly idle network connections and does little computation per request. Which model fits best?

* [x] An asyncio event loop
* [ ] One OS thread per connection
* [ ] One process per connection
* [ ] A process pool sized to the CPU count

> **Explanation:** Async tasks are much cheaper than threads or processes, and one event loop can wait on many sockets at once. CPU-heavy work would be better offloaded to workers.
