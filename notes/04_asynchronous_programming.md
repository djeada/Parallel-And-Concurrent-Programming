## Asynchrony

Asynchronous programming is a technique used to achieve concurrency, where tasks can be executed independently without waiting for other tasks to finish. It allows for nonblocking behavior, in contrast to synchronous execution that waits for one task to complete before starting the next task.

### Building Blocks of Asynchronous Programming

Asynchronous programming offers non-blocking execution, which is especially beneficial for I/O-bound operations. The two main pillars of this paradigm are the event loop and async functions.

#### Function vs Corutine

A **function** is a reusable block of code that performs a specific task. It can take inputs, called **arguments**, and may return an output that the rest of the program can use.

Functions make programs easier to build and maintain by breaking large problems into smaller, clearer steps. In most cases, when a function is called, the program enters the function, runs its instructions, and waits until the function finishes before moving on. The function ends when it reaches its last line or a `return` statement.

A **coroutine** is similar to a function, but with one important difference: it can pause during execution and continue later from the same point. This is useful for tasks that spend time waiting, such as reading a file, loading data from a server, or waiting for user input.

Coroutines are often used in **asynchronous programming**. Instead of blocking the entire program while one task waits, a coroutine can pause and allow other work to happen. Once the needed resource or event is ready, the coroutine resumes where it left off.

Depending on the programming language, coroutines may use keywords such as `await` or `yield` to pause execution and hand control back to the caller or scheduler.

A regular function runs from start to finish when called, while a coroutine can pause, let other tasks run, and then continue later while remembering its previous state. This makes coroutines especially useful for writing efficient programs that handle multiple waiting tasks at the same time.


```
Function:                         Coroutine:
+-------------------+             +-------------------+
|       Start       |             |       Start       |
+-------------------+             +-------------------+
           |                                 |
           v                                 v
+-------------------+             +-------------------+
|   Execute Task    |             |   Execute Part 1  |
+-------------------+             +-------------------+
           |                                 |
           v                                 v
+-------------------+             +-------------------+
|       End         |             |       Pause       |
+-------------------+             +-------------------+
                                             |
                                             v
                                  +-------------------+
                                  |   Execute Part 2  |
                                  +-------------------+
                                             |
                                             v
                                  +-------------------+
                                  |       End         |
                                  +-------------------+
```

#### Event Loop

The **event loop** is the part of an asynchronous program that keeps everything moving. It continually checks which tasks are ready to run and gives them a chance to execute.

For example, a program may be waiting for a file to load, a network request to finish, or a timer to expire. Instead of stopping the entire program until one of these tasks is complete, the event loop allows other work to continue in the meantime.

The event loop is responsible for scheduling tasks, handling completed I/O operations, and managing timers or timeouts. When an operation finishes or a scheduled time is reached, it ensures that the appropriate task runs.

This allows a program to handle multiple tasks efficiently, even when it is running on a single thread. The tasks are not necessarily executing at the exact same moment; instead, the event loop quickly switches between tasks whenever one is waiting, helping the program stay responsive and avoid unnecessary delays.


```
                        ┌──────────────────────────────┐
                        │      JavaScript Code Runs    │
                        │       on the Call Stack      │
                        └──────────────┬───────────────┘
                                       │
                          starts async operation
                                       ▼
        ┌──────────────────────────────────────────────┐
        │          Node.js Runtime / libuv             │
        │                                              │
        │ timers • sockets • file-system work          │
        │ subprocesses • other async operations        │
        └───────────────────┬──────────────────────────┘
                            │
                     work becomes ready
                            ▼
        ┌──────────────────────────────────────────────┐
        │              Ready Queues                    │
        │                                              │
        │ `process.nextTick()` queue                   │
        │ Promise / `queueMicrotask()` queue           │
        │ Event-loop phase callbacks:                  │
        │ timers • poll/I/O • check • close            │
        └───────────────────┬──────────────────────────┘
                            ▼
        ┌──────────────────────────────────────────────┐
        │             Node.js Event Loop               │
        │                                              │
        │ Runs ready JavaScript callbacks on the       │
        │ call stack according to queue/phase rules    │
        └───────────────────┬──────────────────────────┘
                            ▼
                  ┌────────────────────┐
                  │     Call Stack     │
                  │ Callback executes  │
                  └─────────┬──────────┘
                            │
                            └──────────────► Repeat cycle
```

#### Futures and Tasks

A **future** is an object that represents a result that is not ready yet. It is often created when an asynchronous operation begins, such as loading data or waiting for a network response.

At first, a future is usually **pending**. Later, it becomes **completed** when the operation succeeds, or **failed** if an error occurs. Once completed, the result can be retrieved; if the operation failed, the error can be handled.

A future does not usually perform the work itself. Instead, it keeps track of the eventual result of work being carried out elsewhere, such as by an event loop or a thread pool.

A **task** is a future that is connected to the execution of a coroutine. When a coroutine is turned into a task, the event loop can schedule it to run alongside other tasks.

A task can be **awaited**, meaning another coroutine can pause until that task finishes without blocking the entire program. While one task is waiting, the event loop can continue running other tasks.

Tasks can also be canceled when they are no longer needed, or grouped together so that a program can wait for several operations to finish. They are also useful for building workflows where one operation depends on the result of another.

A **future** represents a result that will become available later, while a **task** represents a coroutine that is actively being scheduled and run to produce a result.

### Asynchrony vs. Multithreading

**Asynchrony** allows a program to work on other tasks while it is waiting for something to finish, such as a network request, file operation, or database response. In asynchronous code, tasks cooperate by pausing at points such as `await`, giving the event loop a chance to run other ready tasks. This is especially useful for programs that handle many I/O operations at the same time, such as downloading many files or serving multiple web requests.

**Threads** work differently. The operating system can pause and resume threads automatically through a process called preemptive scheduling. Depending on the programming language and runtime, threads may also allow work to run in parallel across multiple CPU cores. For CPU-heavy tasks, such as image processing or data compression, threads or processes are often more suitable than async alone.

Many real applications use both approaches together. For example, a web server may use asynchronous code to manage many client connections, while sending CPU-intensive or blocking work to a pool of worker threads or processes.

**Concurrency** means making progress on multiple tasks during the same period of time. The tasks do not need to run at exactly the same moment; they may simply take turns. For example, a spreadsheet application can save a file in the background while still responding to user input.

**Parallelism** means that multiple tasks are actually running at the same time, usually on different CPU cores. For example, a video encoder may process several frames simultaneously to finish the job faster.

You can think about it in this way, **async is mainly about avoiding unnecessary waiting**, while **threads or processes can help perform multiple pieces of work at the same time**.

**I. Synchronous Code on a Single Thread**

```text
single thread: AAAA BBBBBBBBBBBBBBBBBBBBBB CCCCCCCCCC
```

In a synchronous, single-threaded program, each task runs from beginning to end before the next task begins. If one task takes a long time, everything behind it has to wait.

**II. Synchronous Code with Multiple Threads**

```text
thread 1: AAAAAAAAAAAA------------------------------
thread 2: ------------BBBBBB------------------------
thread 3: ------------------CCCCCCCCCCCCCCCCCCCCCCCC
```

With multiple threads, each thread can be scheduled independently by the operating system. If multiple CPU cores are available, some tasks may be able to run in parallel.

**III. Asynchronous Code on a Single Thread**

```text
single thread: AAAA----BBBB----AA--CCC--AA--BBBB----CCCC
```

In an asynchronous program, a task can pause when it is waiting for something, such as a response from a server. While that task is paused, the event loop can run another task that is ready.

This allows several tasks to make progress efficiently on a single thread. However, it does not mean that CPU-heavy work is running in parallel.

**IV. Asynchronous Code with Worker Threads or Processes**

```text
thread 1 (event loop): AAAAAA--A--A--C--A--B--C--A------
thread 2 (worker):     ---BBBBBBBB-----------------------
thread 3 (worker):     -----------CCCCCCCC--------------
```

A program can combine async code with worker threads or processes. The event loop handles waiting-based tasks, such as network requests, while blocking or CPU-intensive work is handed off to workers.

This approach is useful for applications that deal with both I/O-heavy and computation-heavy work.

#### Quick Comparison

| Aspect          | Asynchrony                                               | Threads                                                           |
| --------------- | -------------------------------------------------------- | ----------------------------------------------------------------- |
| Scheduling      | Cooperative: tasks pause at `await` or callbacks         | Preemptive: the operating system schedules threads                |
| Best suited for | Many I/O-bound tasks, such as network or file operations | Blocking work and, depending on the runtime, CPU-heavy work       |
| Resource cost   | Usually lightweight                                      | Usually heavier because each thread requires additional resources |
| Common risks    | Blocking the event loop can freeze progress              | Shared state can cause race conditions or deadlocks               |
| Common tools    | Event loops, coroutines, futures, tasks                  | Locks, queues, atomics, thread-safe data structures               |

#### Choosing the Right Approach

For workloads that are mostly **I/O-bound**, async is often a good choice. A web scraper, for example, can send many requests without waiting for each response one at a time.

For workloads that are mostly **CPU-bound**, threads or processes are often more appropriate. Tasks such as image rendering, video processing, or large calculations usually benefit from work being spread across available CPU resources.

For **mixed workloads**, a combination works well. An API server might handle incoming requests asynchronously while sending compression, encryption, or data-processing work to worker threads or processes.

#### Practical Tips

Avoid calling blocking operations directly inside an async event loop. A slow blocking database call or file operation can prevent every other async task from making progress. When necessary, move blocking work to a worker pool.

Use **structured concurrency**, such as task groups, to keep related tasks together. This makes it easier to cancel work, handle errors, and clean up resources correctly.

Apply **timeouts**, **cancellation**, and **backpressure** when dealing with many requests or long-running operations. These techniques help prevent a program from becoming overloaded or waiting forever.

In thread-based programs, reduce shared mutable state whenever possible. Passing messages through queues or using immutable data can make programs safer and easier to reason about.

### Challenges and Considerations

* In **asynchronous programming**, coroutines cooperate by pausing at points such as `await`, allowing the event loop to run other ready tasks. Unlike threads, async tasks are not normally interrupted at arbitrary moments by the operating system.
* Async code can reduce the need for traditional thread locks when tasks share state only within a single event loop and update it without pausing midway. However, synchronization may still be needed when multiple coroutines access shared resources across `await` points, or when threads and processes are involved.
* Switching between async tasks is usually lightweight compared with switching between operating system threads. This makes async especially useful for applications that manage many waiting operations at once, such as a web server handling thousands of network connections.
* Synchronous or blocking functions must be used carefully in async programs. A blocking database query, file operation, or HTTP request can stop the event loop from running other tasks, reducing responsiveness and throughput.
* In Python, blocking work can often be moved out of the event loop with tools such as `asyncio.to_thread()` or `loop.run_in_executor()`. CPU-intensive work is generally better handled with processes, or with threads only when the underlying code can run in parallel outside normal Python execution.
* Errors inside coroutines need to be handled deliberately. A failed task that is ignored can lead to missing results, warnings, incomplete work, or wider failures in the application.
* When several async operations depend on one another, error handling should be structured so that failures are detected and cleaned up consistently. For example, if one step in a payment workflow fails, related tasks may need to be canceled and the transaction safely rolled back.
* Debugging async code can be more difficult because tasks may pause and resume in an order that is not obvious from reading the source code. Clear task names, useful logs, timeouts, and structured concurrency can make the program easier to understand.
* In Python's `asyncio`, there is no `asyncio.debug()` function. Instead, debug mode can be enabled with `asyncio.run(..., debug=True)`, `loop.set_debug(True)`, the `PYTHONASYNCIODEBUG` environment variable, or Python development mode. Debug mode can help identify slow callbacks and incorrectly managed async resources. ([Python documentation][1])
* Async programming is best suited to **I/O-bound** work, such as network requests, file access, database queries, and user interactions. It does not make CPU-heavy calculations run faster by itself.
* A machine learning training loop, image-rendering job, or video-encoding task is unlikely to benefit much from async alone because the bottleneck is computation rather than waiting for I/O.
* A **hybrid approach** is often the most practical solution for mixed workloads. For example, a video streaming service might use async code for network connections while using worker processes or specialized libraries for video encoding.
* **Backpressure** is needed when work is arriving faster than it can be processed. Without it, queues can grow uncontrollably, memory usage can increase, and response times can become unacceptable.
* Buffer limits, bounded queues, throttling, timeouts, and load shedding are common ways to manage overload. For example, a streaming application may reduce buffering or drop lower-priority work rather than allowing delays to grow without limit.
* Testing async code requires attention to timing, cancellation, timeouts, and the interaction between concurrent tasks. Tests should check both successful execution and failure scenarios, such as a request timing out or one task being canceled.
* In Python, tools such as **pytest-asyncio** can run coroutine-based tests and help verify async workflows. Tests can confirm that dependent operations happen in the correct order, failures are handled correctly, and resources are cleaned up after cancellation.

[1]: https://docs.python.org/3/library/asyncio-dev.html

### Typical Applications

| Use case                                           | Pattern                                                                        | C++: good fit                                                                                        | Python: good fit                                                                                              | Why it fits                                                                                                | Shutdown considerations                                                                                                                                  |
| -------------------------------------------------- | ------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------- |
| High-concurrency HTTP **client** (scraper/crawler) | Send many requests concurrently while limiting in-flight work                  | C++20 coroutines with Boost.Asio + Boost.Beast; add a concurrency limit                              | `asyncio` + `aiohttp`; use connection limits and/or `asyncio.Semaphore`                                       | Network requests spend much of their time waiting, so async can overlap that waiting efficiently           | Stop creating new requests, cancel or finish pending tasks, and close the HTTP session cleanly. ([Boost][1])                                             |
| High-concurrency HTTP **server** (API)             | Accept and handle many connections through an event loop                       | Boost.Asio + Boost.Beast coroutines                                                                  | FastAPI or Starlette with an async ASGI server                                                                | Async servers handle many mostly waiting connections without requiring one thread per client               | Stop accepting new requests, allow in-flight work to finish within a deadline, and release resources in the application lifespan handler. ([FastAPI][2]) |
| WebSocket chat or broadcasting                     | Maintain long-lived two-way connections and distribute messages                | Boost.Asio + Boost.Beast WebSockets                                                                  | `websockets` or `aiohttp` WebSockets                                                                          | WebSockets often remain open while idle, making them a natural fit for async I/O                           | Stop producers, flush important outgoing messages where practical, send close frames, and close connections                                              |
| Reverse proxy or API gateway                       | Stream requests and responses between clients and upstream services            | Boost.Asio + Boost.Beast streaming operations                                                        | Async HTTP client/server libraries with streaming support                                                     | Streaming avoids loading entire bodies into memory and supports flow control                               | Stop accepting traffic, cancel or drain forwarding tasks, close upstream connections, and preserve partial-failure handling                              |
| Streaming pipeline                                 | Move data through stages such as socket → transform → sink                     | Coroutines connected with bounded queues                                                             | Tasks connected with `asyncio.Queue(maxsize=...)`                                                             | Bounded queues create backpressure and keep memory use controlled                                          | Stop intake, signal completion or cancel tasks, drain required output, and await remaining stages                                                        |
| Database access with an async driver               | Use pooled connections and await queries or transactions                       | Database-specific async client integrated with Asio, such as Boost.MySQL for MySQL                   | An async driver or async ORM layer, such as `asyncpg` or SQLAlchemy async                                     | Async database access prevents query waits from blocking the event loop                                    | Stop new transactions, allow or cancel active queries according to policy, roll back incomplete transactions, and close the pool                         |
| Periodic jobs or heartbeats                        | Repeat work with an asynchronous timer or sleep                                | Boost.Asio `steady_timer` with coroutines                                                            | A coroutine loop using `asyncio.sleep()`; manage related jobs with `TaskGroup`                                | Timers are inexpensive and can be canceled cooperatively                                                   | Cancel the timer or task, optionally complete a final update, and await cleanup. ([Python documentation][3])                                             |
| RPC client with retries and timeouts               | Apply a timeout per call and retry selected failures                           | Boost.Asio coroutines with timers and cancellation                                                   | `asyncio.timeout()` with a controlled retry loop                                                              | Timeouts and retries compose naturally around awaited operations                                           | Stop retry loops during shutdown, cancel in-flight calls, and close transports. ([Python documentation][3])                                              |
| Bulk cloud uploads or downloads                    | Transfer many objects with bounded concurrency                                 | Coroutines with the relevant HTTP or cloud SDK                                                       | An async cloud SDK, or `aiohttp` where appropriate                                                            | Async overlaps network latency while limits prevent excessive memory or bandwidth use                      | Decide whether to finish or checkpoint active transfers, cancel queued work, and close clients                                                           |
| Batch DNS lookups                                  | Resolve many hostnames concurrently                                            | Boost.Asio asynchronous resolver                                                                     | Event-loop DNS APIs or a compatible async resolver                                                            | DNS requests are waiting-heavy operations that can be issued concurrently                                  | Cancel outstanding requests where supported, close resolver resources, and retain valid cached results if needed                                         |
| Message queue consumers or producers               | Consume, acknowledge, and publish messages asynchronously                      | Async-capable AMQP or Kafka client integrated with the runtime                                       | Async AMQP or Kafka client library                                                                            | Broker interactions are mostly I/O-bound and often need explicit flow control                              | Stop consuming first, finish or reject in-flight messages according to policy, flush publishes, and close channels                                       |
| GUI application with network activity              | Keep the user interface responsive while performing I/O                        | Qt event-loop integration with coroutines or carefully managed workers                               | Qt event-loop integration with async code, or workers for blocking calls                                      | Network waits should not freeze the UI thread                                                              | Cancel background work before closing the window, detach callbacks safely, and close network resources                                                   |
| IoT gateway with many devices                      | Maintain large numbers of mostly idle TCP or MQTT sessions                     | Boost.Asio coroutines; serialize shared session state where needed                                   | `asyncio` sockets with an async MQTT client where appropriate                                                 | Async handles large numbers of waiting device connections efficiently                                      | Stop accepting devices, unsubscribe where required, persist state or offsets, and close sockets                                                          |
| Rate-limited API worker                            | Limit request rate and the number of concurrent operations                     | Boost.Asio timers with a token-bucket or leaky-bucket limiter                                        | `asyncio` or AnyIO with a rate limiter and concurrency bound                                                  | Rate limiting smooths bursts and helps avoid server throttling                                             | Stop admitting new work, cancel waiting callers, finish or discard queued work deliberately, and stop refill timers                                      |
| File-to-socket streaming                           | Read file data and send it over a network connection without blocking the loop | Platform-supported async file I/O where available, otherwise offload file access from the event loop | `asyncio` streams plus `asyncio.to_thread()` for ordinary blocking file operations, or a suitable wrapper     | Socket I/O is naturally async, but regular file I/O is not uniformly non-blocking across platforms         | Flush required buffers, close file handles and sockets, and complete or cancel worker operations safely. ([Python documentation][4])                     |
| Bulk email or SMS sending                          | Send many independent messages through slow external services                  | Async HTTP or SMTP client integrated with the runtime                                                | Async provider SDK or async SMTP client                                                                       | Delivery requests are usually I/O-bound and can be bounded and retried                                     | Stop new submissions, finish or persist queued messages, record failures, and close clients                                                              |
| Health checks or monitoring fan-out                | Probe many endpoints on a schedule                                             | Boost.Asio coroutines with timers                                                                    | `asyncio` + async HTTP client; use `TaskGroup` for related probes                                             | A large number of short I/O checks can run concurrently with low overhead                                  | Cancel probes during shutdown, keep partial results if useful, and close the client session                                                              |
| Lightweight multiplayer or session server          | Exchange frequent small messages with many connected clients                   | Boost.Asio UDP or TCP operations with coroutines                                                     | `asyncio` UDP/TCP APIs or streams                                                                             | Async can keep connection and session management responsive when per-message work is small                 | Notify or disconnect clients, flush critical outbound messages, persist session state if needed, and close transports                                    |
| Metrics collection and forwarding                  | Receive metrics, batch them, and forward them periodically                     | Boost.Asio UDP/TCP handling with batch timers                                                        | `asyncio` datagrams or streams with batch sending                                                             | Batching reduces overhead while async keeps ingestion responsive                                           | Stop intake, flush the final batch within a deadline, and close sockets                                                                                  |
| Subprocess orchestration                           | Launch processes, read output, and apply timeouts                              | Boost.Process with async pipes and the chosen event system                                           | `asyncio.create_subprocess_exec()`                                                                            | Async prevents stdout and stderr handling from blocking supervision logic                                  | Terminate or kill overdue processes, drain pipes where needed, and await process completion. ([Python documentation][5])                                 |
| Webhook or event-processing workers                | Receive bursts of events and dispatch work safely                              | Async HTTP server with a bounded work queue                                                          | FastAPI with bounded background work or a durable external queue                                              | Async handles incoming I/O well, while durable queues are safer for work that must survive restarts        | Stop accepting events, persist unprocessed work when required, finish tasks within a deadline, and shut down dependencies                                |
| Background task that is not immediately awaited    | Start work now but keep ownership of it                                        | Coroutine stored in a supervisor or task registry                                                    | `asyncio.create_task()` with a stored task reference, or preferably a `TaskGroup` when the lifetime is scoped | Background work remains manageable only when it is tracked and failures are observed                       | Keep task references, capture errors, cancel or await tasks during shutdown. ([Python documentation][3])                                                 |
| Calling blocking code from async code              | Prevent a blocking library call from freezing the event loop                   | Run the blocking operation on a controlled worker pool                                               | `asyncio.to_thread()` for blocking I/O; use a process pool for CPU-heavy Python work when appropriate         | Blocking I/O must leave the event-loop thread; CPU-heavy work generally needs separate execution resources | Bound the worker pool, stop admitting new calls, await or abandon work according to policy, and shut down executors cleanly. ([Python documentation][4]) |

[1]: https://www.boost.org/doc/libs/latest/libs/beast/doc/html/index.html "Chapter 1. Boost.Beast"
[2]: https://fastapi.tiangolo.com/advanced/events/?utm_source=chatgpt.com "Lifespan Events"
[3]: https://docs.python.org/3/library/asyncio-task.html "Coroutines and tasks — Python 3.14.5 documentation"
[4]: https://docs.python.org/3/library/asyncio-eventloop.html "Event loop — Python 3.14.5 documentation"
[5]: https://docs.python.org/3/library/asyncio-subprocess.html "Subprocesses — Python 3.14.5 documentation"

### Examples

#### Examples in C++

Asynchronous programming in C++ involves performing tasks without blocking the main thread, allowing other operations to continue in parallel. This can be particularly useful for I/O-bound or computationally intensive tasks. The C++ Standard Library provides facilities for asynchronous programming, including the `std::async` function, `std::future`, and `std::promise`.

##### Asynchronous Tasks with `std::async`

The `std::async` function runs a function asynchronously, returning a `std::future` that will eventually hold the result of the function.

```cpp
#include <iostream>
#include <future>
#include <thread>

int compute(int x) {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate a long computation
    return x * x;
}

int main() {
    std::future<int> future = std::async(std::launch::async, compute, 5);

    std::cout << "Doing other work while waiting for the result..." << std::endl;

    int result = future.get(); // Wait for the result
    std::cout << "Result: " << result << std::endl;

    return 0;
}
```

In this example, `std::async` launches the `compute` function asynchronously, allowing the main thread to continue executing other code. The `future.get()` method waits for the result and retrieves it once the computation is complete.

##### Using `std::future` and `std::promise`

`std::future` and `std::promise` provide a way to communicate between threads asynchronously. A `std::promise` object is used to set a value that will be available to a `std::future` object.

```cpp
#include <iostream>
#include <future>
#include <thread>

void set_value(std::promise<int>& promise) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    promise.set_value(10); // Set the value to be retrieved by future
}

int main() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

    std::thread t(set_value, std::ref(promise));
    
    std::cout << "Waiting for value..." << std::endl;
    int value = future.get(); // Wait for the value
    std::cout << "Value: " << value << std::endl;

    t.join();
    return 0;
}
```

In this example, the `set_value` function sets a value to the promise, which is then retrieved by the future in the main thread.

##### Asynchronous Waiting with `std::future`

The `std::future` object provides a way to wait for a result that is being computed asynchronously.

```cpp
#include <iostream>
#include <future>
#include <thread>

int slow_add(int a, int b) {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return a + b;
}

int main() {
    std::future<int> future = std::async(std::launch::async, slow_add, 3, 4);

    while (future.wait_for(std::chrono::milliseconds(500)) != std::future_status::ready) {
        std::cout << "Waiting for the result..." << std::endl;
    }

    std::cout << "Result: " << future.get() << std::endl;
    return 0;
}
```

Here, `future.wait_for` periodically checks if the result is ready, allowing the main thread to perform other tasks while waiting.

##### Exception Handling with Asynchronous Operations

Exceptions thrown in asynchronous tasks are captured by `std::future` and can be re-thrown when `future.get()` is called.

```cpp
#include <iostream>
#include <future>
#include <stdexcept>

int risky_task() {
    throw std::runtime_error("Something went wrong");
    return 42; // This line won't be reached
}

int main() {
    std::future<int> future = std::async(std::launch::async, risky_task);

    try {
        int result = future.get();
        std::cout << "Result: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}
```

In this code, the exception thrown in `risky_task` is caught in the main thread when `future.get()` is called.

##### Asynchronous Data Sharing with `std::shared_future`

A `std::shared_future` allows multiple threads to share the result of an asynchronous operation.

```cpp
#include <iostream>
#include <future>
#include <thread>
#include <vector>

int compute_value() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 10;
}

int main() {
    std::shared_future<int> shared_future = std::async(std::launch::async, compute_value).share();

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([shared_future, i]() {
            std::cout << "Thread " << i << ": " << shared_future.get() << std::endl;
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
```

In this example, the result of `compute_value` is shared among multiple threads using `std::shared_future`.

##### Asynchronous Continuations with `std::future`

Asynchronous continuations allow chaining of asynchronous operations.

> **⚠️ Note:** `std::future` does **not** have a `.then()` method in standard C++ (C++11–C++23). Such code will not compile. The `.then()` continuation API was proposed in the Concurrency TS (N4538) but was never merged into the standard. Libraries such as Folly, HPX, and Boost.Future provide it as an extension.

The standard way to sequence async work is to call `.get()` and pass the result manually:

```cpp
#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int initial_task() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 5;
}

int next_task(int input) {
    return input * 2;
}

int main() {
    // Launch first task
    auto first = std::async(std::launch::async, initial_task);

    // Block for the result, then pass it to the next task
    int intermediate = first.get();
    auto second = std::async(std::launch::async, next_task, intermediate);

    std::cout << "Result: " << second.get() << std::endl;  // prints 10
    return 0;
}
```

For true non-blocking chaining, use C++20 coroutines (see `coroutine_task.cpp`) or a library such as Boost.Asio.

##### Performance Considerations and Best Practices

- Use asynchronous operations for long-running tasks to keep the main thread responsive.
- Ensure that exceptions are properly handled in asynchronous tasks to prevent crashes.
- Be cautious with resources shared between asynchronous tasks; use synchronization mechanisms like mutexes if necessary.
- Creating too many threads can be costly. Use asynchronous mechanisms and thread pools where appropriate.
- Leverage C++11 and later features like `std::async`, `std::future`, and `std::promise` for cleaner and more efficient asynchronous programming.

| No. | Filename                                                                                         | Description                                              |
|-----|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| 1   | [basic_async.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/basic_async.cpp) | Create and start a basic asynchronous task              |
| 2   | [parallel_futures.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/parallel_futures.cpp) | Launch multiple parallel tasks and collect results via `std::future` |
| 3   | [future.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/future.cpp) | Read the result of a completed `std::future`             |
| 4   | [shared_future.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/shared_future.cpp) | Broadcast one result to many consumers with `std::shared_future` |
| 5   | [future_timed_wait.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/future_timed_wait.cpp) | Timed waits with `wait_for`/`wait_until`/`future_status` |
| 6   | [async_exception.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/async_exception.cpp) | Exception propagation through futures and `promise::set_exception` |
| 7   | [async_antipatterns.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/async_antipatterns.cpp) | ⚠️ ANTIPATTERN: forgotten future, default launch policy, double `.get()` |
| 8   | [promise_future.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/promise_future.cpp) | Use promises and futures to pass values between tasks    |
| 9   | [packaged_task.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/packaged_task.cpp) | Wrap a callable with `std::packaged_task` for deferred execution |
| 10  | [thread_pool_futures.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/thread_pool_futures.cpp) | Thread pool with `packaged_task` and future-based interface |
| 11  | [pause_resume.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/pause_resume.cpp) | Pause and resume asynchronous tasks                      |
| 12  | [heavy_functions.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/heavy_functions.cpp) | Execute heavy functions asynchronously                   |
| 13  | [async_producer_consumer.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/async_producer_consumer.cpp) | Implement a producer-consumer pattern asynchronously     |
| 14  | [recursive_sum.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/recursive_sum.cpp) | Parallel divide-and-conquer sum with `std::async`        |
| 15  | [async_semaphore.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/async_semaphore.cpp) | Control access to shared resources with a semaphore      |
| 16  | [async_barrier.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/async_barrier.cpp) | Synchronize multiple asynchronous tasks using a barrier  |
| 17  | [coroutine_generator.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/coroutine_generator.cpp) | C++20 lazy generator with `co_yield`                     |
| 18  | [coroutine_task.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/coroutine_task.cpp) | C++20 `co_await` coroutine chaining with a minimal `Task<T>` |
| 19  | [async_mutex.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/async_mutex.cpp) | Use a mutex to synchronize access to shared resources    |
| 20  | [parallel_fetch.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/parallel_fetch.cpp) | Fetch data in parallel (requires libcurl)                |
| 21  | [async_server.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/async_server.cpp) | ⚠️ ANTIPATTERN: detached-thread-per-connection server    |
| 22  | [distributed_computing.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/cpp/asynchrony/distributed_computing.cpp) | Distributed computing with socket server/worker/client  |

#### Examples in Python

In Python, asynchronous programming allows you to run code concurrently without creating new threads or processes. This is especially useful for I/O-bound tasks, such as network requests or file operations, where waiting for an operation to complete would otherwise block other code from running. Python's `asyncio` library provides the primary framework for writing asynchronous code using the `async` and `await` keywords.

##### Asynchronous Functions

An asynchronous function is defined using the `async def` syntax. These functions return a coroutine, which is an object representing the eventual result of the function.

```python
import asyncio

async def print_message(message):
    print(message)

# Example usage
async def main():
    await print_message("Hello from an asynchronous function!")

asyncio.run(main())
```

In this example, `print_message` is an asynchronous function that prints a message. The `main` function calls it with `await`, indicating it will wait for the coroutine to complete.

##### Running Asynchronous Tasks

You can run multiple asynchronous tasks concurrently using `asyncio.create_task()` or by awaiting multiple coroutines.

```python
import asyncio

async def print_message(message):
    await asyncio.sleep(1)
    print(message)

async def main():
    task1 = asyncio.create_task(print_message("Task 1"))
    task2 = asyncio.create_task(print_message("Task 2"))
    await task1
    await task2

asyncio.run(main())
```

Here, `asyncio.create_task()` schedules the coroutines to run concurrently. The `await` statements ensure that `main` waits for both tasks to finish.

##### Handling Concurrent I/O Operations

Asynchronous functions are ideal for I/O-bound tasks where blocking operations can slow down the program. For example, making HTTP requests.

```python
import asyncio
import aiohttp

async def fetch(url):
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as response:
            return await response.text()

async def main():
    url = "http://example.com"
    html = await fetch(url)
    print(html)

asyncio.run(main())
```

In this example, `fetch` uses `aiohttp` for asynchronous HTTP requests. This allows multiple requests to be handled without blocking the event loop.

##### Awaiting Multiple Tasks

You can run multiple coroutines concurrently using `await asyncio.gather()`.

```python
import asyncio

async def fetch_data(n):
    await asyncio.sleep(n)
    return f"Data {n}"

async def main():
    results = await asyncio.gather(
        fetch_data(1),
        fetch_data(2),
        fetch_data(3)
    )
    print(results)

asyncio.run(main())
```

`asyncio.gather()` waits for all the provided coroutines to finish and returns their results in a list.

##### Using Async Context Managers

Async context managers, defined with `async with`, are used to manage resources that require cleanup after use.

```python
import asyncio
import aiofiles

async def write_to_file(filename, content):
    async with aiofiles.open(filename, 'w') as file:
        await file.write(content)

async def main():
    await write_to_file('example.txt', 'Hello, Async World!')

asyncio.run(main())
```

In this example, `aiofiles` is used for asynchronous file operations. The file is automatically closed after writing.

##### Exception Handling in Asynchronous Code

Handling exceptions in asynchronous code is similar to synchronous code, using try-except blocks.

```python
import asyncio

async def may_raise_exception():
    await asyncio.sleep(1)
    raise ValueError("An error occurred!")

async def main():
    try:
        await may_raise_exception()
    except ValueError as e:
        print(f"Caught an exception: {e}")

asyncio.run(main())
```

Here, the exception raised in `may_raise_exception` is caught and handled in `main`.

##### Asynchronous Iteration

Asynchronous iterators and iterables allow for asynchronous looping, useful when dealing with streams or large datasets.

```python
import asyncio

class AsyncCounter:
    def __init__(self, limit):
        self.limit = limit
        self.count = 0

    async def __aiter__(self):
        return self

    async def __anext__(self):
        if self.count < self.limit:
            await asyncio.sleep(1)
            self.count += 1
            return self.count
        else:
            raise StopAsyncIteration

async def main():
    async for number in AsyncCounter(3):
        print(number)

asyncio.run(main())
```

In this example, `AsyncCounter` is an asynchronous iterable that generates numbers with a delay.

##### Performance Considerations and Best Practices

- Ensure that all time-consuming or blocking calls are made asynchronously. This keeps the event loop responsive.
- Use async context managers to manage resources, such as files or network connections, ensuring proper cleanup.
- Handle exceptions carefully to avoid silent failures, especially when dealing with multiple concurrent tasks.
- Be mindful of task cancellation. Use `try...finally` blocks to clean up resources if a task is canceled.
- Use mechanisms like semaphores to limit the number of concurrent tasks if interacting with rate-limited resources or APIs.

| No. | Filename                                                                                         | Description                                              |
|-----|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| 1   | [basic_async.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/basic_async.py) | Create and start a basic asynchronous task              |
| 2   | [future_create_task.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/future_create_task.py) | Create a task using Future and run it asynchronously     |
| 3   | [future_callback.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/future_callback.py) | Use callbacks with Future objects               |
| 4   | [pause_resume.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/pause_resume.py) | Pause and resume asynchronous tasks                      |
| 5   | [run_heavy_functions.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/run_heavy_functions.py) | Execute heavy functions asynchronously                   |
| 6   | [data_sharing_queue.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/data_sharing_queue.py) | Share data between asynchronous tasks using a Queue      |
| 7   | [semaphore.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/semaphore.py) | Control access to shared resources with a Semaphore      |
| 8   | [producer_consumer.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/producer_consumer.py) | Implement a producer-consumer pattern asynchronously     |
| 9   | [fetch_parallel.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/fetch_parallel.py) | Fetch data in parallel using async tasks                 |
| 10  | [mutex.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/mutex.py) | Use a Mutex to synchronize access to shared resources    |
| 11  | [barrier.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/barrier.py) | Synchronize multiple asynchronous tasks using a Barrier |
| 12  | [async_generator.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/async_generator.py) | Create and use asynchronous generators                   |
| 13  | [async_server.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/async_server.py) | Implement an asynchronous server                         |
| 14  | [distributed_computing.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/main/src/python/asynchrony/distributed_computing.py) | Demonstrate distributed computing with async tasks       |

#### Examples in JavaScript

Node.js is inherently designed for asynchronous programming, primarily due to its non-blocking I/O model. It uses an event-driven architecture, which allows for efficient handling of concurrent operations. Asynchronous code in Node.js can be written using callbacks, Promises, and the `async`/`await` syntax.

##### Asynchronous Programming with Callbacks

Callbacks are the traditional way of handling asynchronous operations in Node.js. A callback is a function passed as an argument to another function, which will be called once the operation is complete.

```javascript
const fs = require('fs');

fs.readFile('example.txt', 'utf8', (err, data) => {
    if (err) {
        console.error('Error reading file:', err);
        return;
    }
    console.log('File contents:', data);
});

console.log('Reading file...');
```

In this example, `fs.readFile` reads a file asynchronously and calls the provided callback function when the operation completes, either with an error or the file's data.

##### Asynchronous Programming with Promises

Promises provide a more elegant way to handle asynchronous operations by avoiding the callback pyramid of doom (nested callbacks). A Promise represents a value that may be available now, or in the future, or never.

```javascript
const fs = require('fs').promises;

fs.readFile('example.txt', 'utf8')
    .then(data => {
        console.log('File contents:', data);
    })
    .catch(err => {
        console.error('Error reading file:', err);
    });

console.log('Reading file...');
```

Here, `fs.promises.readFile` returns a Promise. The `then` method handles the resolved value, while `catch` handles any errors.

##### Asynchronous Programming with `async`/`await`

The `async`/`await` syntax in Node.js (available from ECMAScript 2017) is syntactic sugar over Promises, making asynchronous code look and behave more like synchronous code.

```javascript
const fs = require('fs').promises;

async function readFile() {
    try {
        const data = await fs.readFile('example.txt', 'utf8');
        console.log('File contents:', data);
    } catch (err) {
        console.error('Error reading file:', err);
    }
}

readFile();
console.log('Reading file...');
```

In this example, `async` declares an asynchronous function, and `await` pauses the execution until the Promise is resolved, making the code more readable and maintainable.

##### Handling Multiple Asynchronous Operations

Node.js provides several methods to handle multiple asynchronous operations concurrently, such as `Promise.all`, `Promise.race`, `Promise.allSettled`, and `Promise.any`.

```javascript
const fetch = require('node-fetch');

async function fetchData() {
    try {
        const [response1, response2] = await Promise.all([
            fetch('https://api.example.com/data1'),
            fetch('https://api.example.com/data2')
        ]);
        
        const data1 = await response1.json();
        const data2 = await response2.json();
        
        console.log('Data 1:', data1);
        console.log('Data 2:', data2);
    } catch (err) {
        console.error('Error fetching data:', err);
    }
}

fetchData();
```

In this example, `Promise.all` waits for all the promises to resolve before continuing. This is useful when you need to perform multiple independent asynchronous operations and handle their results together.

##### Error Handling in Asynchronous Code

Proper error handling is crucial in asynchronous programming to avoid crashes and ensure reliability.

```javascript
async function getData() {
    try {
        let response = await fetch('https://api.example.com/data');
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        let data = await response.json();
        return data;
    } catch (err) {
        console.error('Error:', err.message);
        // Additional error handling logic
    }
}

getData();
```

In this example, the error is caught using a try-catch block, allowing for graceful handling of potential failures.

##### Asynchronous Iteration with `for-await-of`

Node.js supports asynchronous iteration using `for-await-of`, which is useful for processing streams of data.

```javascript
const fs = require('fs');
const readline = require('readline');

async function processLineByLine() {
    const fileStream = fs.createReadStream('example.txt');
    const rl = readline.createInterface({
        input: fileStream,
        crlfDelay: Infinity
    });

    for await (const line of rl) {
        console.log(`Line from file: ${line}`);
    }
}

processLineByLine();
```

Here, `for-await-of` iterates over each line of a file asynchronously, making it easier to work with data that comes in pieces, like streams.

##### Avoiding Callback Hell

To avoid deeply nested callbacks (callback hell), use Promises or the `async`/`await` syntax. For example:

```javascript
// Callback Hell Example
function doSomething(callback) {
    fs.readFile('file1.txt', 'utf8', (err, data1) => {
        if (err) return callback(err);
        fs.readFile('file2.txt', 'utf8', (err, data2) => {
            if (err) return callback(err);
            callback(null, data1 + data2);
        });
    });
}

// Using Promises or async/await
async function doSomethingBetter() {
    try {
        const data1 = await fs.readFile('file1.txt', 'utf8');
        const data2 = await fs.readFile('file2.txt', 'utf8');
        return data1 + data2;
    } catch (err) {
        console.error('Error:', err);
    }
}
```

Using Promises or `async`/`await` leads to cleaner and more maintainable code compared to deeply nested callbacks.

##### Performance Considerations and Best Practices

- Always handle errors in asynchronous code to prevent unhandled rejections and crashes.
- Be mindful of concurrency, especially when interacting with rate-limited APIs or performing operations that can overwhelm system resources.
- Watch out for memory leaks, particularly when dealing with streams or large datasets.
- Use tools and techniques like `async_hooks`, `Node.js inspector`, and unit testing to debug and test asynchronous code effectively.

| No. | Filename                                                                                         | Description                                              |
|-----|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| 1   | [01_basic_async.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/01_basic_async.js) | Create and start a basic asynchronous task              |
| 2   | [02_future_create_task.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/02_future_create_task.js) | Create a task using Future and run it asynchronously     |
| 3   | [03_future_callback.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/03_future_callback.js) | Read the result of a completed Future task               |
| 4   | [04_pause_resume.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/04_pause_resume.js) | Pause and resume asynchronous tasks with setInterval     |
| 5   | [05_run_heavy_functions.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/05_run_heavy_functions.js) | Execute heavy functions asynchronously with setImmediate yielding |
| 6   | [06_data_sharing_queue.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/06_data_sharing_queue.js) | Share data between async tasks using a bounded async queue |
| 7   | [07_semaphore.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/07_semaphore.js) | Limit concurrency with a Promise-based semaphore         |
| 8   | [08_producer_consumer.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/08_producer_consumer.js) | Producer-consumer with async queue and poison-pill termination |
| 9   | [09_fetch_parallel.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/09_fetch_parallel.js) | Sequential vs parallel HTTP fetch; `Promise.allSettled` |
| 10  | [10_mutex.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/10_mutex.js) | Protect shared state with a Promise-based async mutex    |
| 11  | [11_barrier.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/11_barrier.js) | Synchronize async tasks across phases with a reusable barrier |
| 12  | [12_async_generator.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/12_async_generator.js) | `async function*`, `for await...of`, transform/filter pipelines |
| 13  | [13_async_server.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/13_async_server.js) | Async HTTP server with route handling and graceful shutdown |
| 14  | [14_distributed_computing.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/14_distributed_computing.js) | Distributed task dispatch over TCP (server / worker / client roles) |
| 15  | [15_promise_combinators.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/15_promise_combinators.js) | `race()` timeout, `any()` first-success, `all()` fail-fast, `allSettled()` |
| 16  | [16_abort_controller.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/16_abort_controller.js) | AbortController, `AbortSignal.timeout()`, `AbortSignal.any()`, fetch cancellation |
| 17  | [17_async_antipatterns.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/17_async_antipatterns.js) | ⚠️ ANTIPATTERN: unhandled rejection, `async` in `forEach`, forgotten `await`, sequential loop |
| 18  | [18_event_loop.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/18_event_loop.js) | Event loop phases: `nextTick` → microtasks → timers → `setImmediate`; starvation |
| 19  | [19_retry_backoff.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/19_retry_backoff.js) | Retry with exponential backoff + jitter; circuit breaker pattern |
| 20  | [20_async_stream.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/20_async_stream.js) | `stream.pipeline()`, `for await` on Readable, manual write+drain backpressure |

### Summary

Here is a table comparing asynchronous programming features in C++, Python, and Node.js:

| Concept                  | C++                                                                                                                                                                                                                                                                                       | Python                                                                                                                                    | Node.js                                                                                                                                                      |
| ------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Main async support**   | The standard library provides `std::future`, `std::promise`, `std::packaged_task`, and `std::async`. C++20 adds coroutine language support. Libraries such as Boost.Asio provide event-loop-based asynchronous I/O.                                                                       | The `asyncio` library provides the event loop, coroutines, futures, tasks, synchronization tools, and async I/O support.                  | JavaScript provides `Promise` and `async`/`await`; Node.js provides asynchronous I/O APIs and runtime support.                                               |
| **Event loop**           | The C++ standard library does not provide a general-purpose event loop. Libraries such as Boost.Asio provide one through `io_context`.                                                                                                                                                    | `asyncio` provides and manages an event loop that schedules callbacks and tasks.                                                          | Node.js uses an event loop for non-blocking I/O; its low-level asynchronous behavior is built around `libuv`.                                                |
| **Coroutine**            | A C++20 coroutine is a function that can suspend and resume using `co_await`, `co_yield`, or `co_return`. A library usually supplies the coroutine return type and scheduling model.                                                                                                      | A function declared with `async def` creates a coroutine object when called. It can suspend with `await`.                                 | An `async` function returns a `Promise` and can pause with `await` until another promise settles.                                                            |
| **Future / Promise**     | `std::future` retrieves a value or exception stored in shared state, often produced by `std::promise`, `std::packaged_task`, or `std::async`.                                                                                                                                             | `asyncio.Future` represents a low-level result that will become available later. It does **not** normally represent a coroutine directly. | A `Promise` represents the eventual completion or failure of an asynchronous operation.                                                                      |
| **Await syntax**         | `co_await` suspends a coroutine until an awaitable operation is ready.                                                                                                                                                                                                                    | `await` pauses a coroutine until an awaitable object completes.                                                                           | `await` pauses an async function until a promise or promise-like value settles.                                                                              |
| **Starting async work**  | `std::async` runs a callable and returns a future, but it is not an event-loop I/O mechanism and may execute later if deferred. With Boost.Asio, coroutines can be started with `co_spawn`.                                                                                               | `asyncio.create_task()` schedules a coroutine to run as a task; `asyncio.gather()` or `TaskGroup` can coordinate multiple operations.     | Calling an async function returns a promise. `Promise.all()` can coordinate multiple operations, while Node I/O APIs begin the underlying asynchronous work. |
| **Task**                 | There is no direct standard-library equivalent of Python's `asyncio.Task`. `std::packaged_task` wraps a callable and connects it to a future, but it does not schedule a coroutine. In Boost.Asio, coroutine work is commonly represented by `awaitable<T>` and launched with `co_spawn`. | `asyncio.Task` wraps and schedules a coroutine on the event loop. It is also a subclass of `Future`.                                      | Node.js does not expose a separate task type equivalent to `asyncio.Task`; async work is usually represented and coordinated through promises.               |
| **Typical use**          | `std::async` and futures are useful for result-oriented concurrent work; Boost.Asio coroutines are suited to asynchronous networking and timers.                                                                                                                                          | Well suited to high-concurrency I/O tasks such as network requests, servers, database calls, and subprocess management.                   | Well suited to servers, file and network I/O, streams, timers, and other event-driven applications.                                                          |
| **Important limitation** | C++ coroutines provide suspension syntax, but they do not automatically provide an event loop, thread pool, or asynchronous I/O implementation.                                                                                                                                           | Blocking synchronous code inside the event loop can prevent other tasks from running.                                                     | Long-running synchronous JavaScript blocks the event loop and delays other requests and callbacks.                                                           |

