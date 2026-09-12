## Multithreading

**Multithreading** is the use of multiple threads within a process so that work can proceed concurrently. A thread is a unit of execution that the operating system can schedule. Threads in the same process share an address space and many process resources, while each thread has its own execution state, such as a stack and registers. Depending on the hardware and runtime, threads may run in parallel on different cores or take turns on the same core. Multithreading is especially useful for I/O-bound work because one thread can make progress while another waits for I/O. Because threads can access shared memory, shared state must be synchronized carefully to avoid data races and other concurrency bugs.

### Thread Pool vs On-Demand Thread

```
+----------------+        +----------------+        +------------------+
| Incoming Tasks |        |  Pool Manager  |        |   Thread Pool    |
|                |        |                |        |                  |
| +-----------+  |        |                |        |  +-----------+   |
| | Task 1    |-------------> Assigns Task ---------> | Thread 1   |   |
| +-----------+  |        |                |        |  +-----------+   |
| +-----------+  |        |                |        |  +-----------+   |
| | Task 2    |-------------> Assigns Task ---------> | Thread 2   |   |
| +-----------+  |        |                |        |  +-----------+   |
| +-----------+  |        |                |        |  +-----------+   |
| | Task 3    |-------------> Assigns Task ---------> | Thread 3   |   |
| +-----------+  |        |                |        |  +-----------+   |
| +-----------+  |        |                |        |  +-----------+   |
| | Task 4    |-------------> Assigns Task ---------> | Thread 4   |   |
| +-----------+  |        |                |        |  +-----------+   |
| +-----------+  |        |                |        +------------------+
| | Task 5    |-------------> Waiting      | 
| +-----------+  |        |                | 
+----------------+        +----------------+
```

* Two common approaches are reusing threads through a thread pool and creating threads on demand.
* A thread pool maintains a set of reusable worker threads, reducing repeated thread-creation overhead and helping control concurrency.
* On-demand spawning creates threads only when needed, which can avoid keeping idle workers around.
* The tradeoff is that creating threads at runtime adds latency and can lead to excessive thread counts if demand is not bounded.

### Worker Threads

* A program often starts with a main thread and creates additional worker threads to perform tasks.
* Worker threads may receive work from the main thread, a task queue, or another coordination mechanism.
* A thread pool can be used to reuse workers and limit how many threads run concurrently.

For example, a web server may place incoming requests in a work queue and let threads from a pool process them. After a worker finishes a request, it becomes available for more work, while the accepting thread can continue handling new connections.

### Advantages of Threads over Processes

* Threads can offer several advantages over separate processes when work belongs within the same application.
* They can improve responsiveness by moving long-running or blocking work away from a thread that must remain interactive.
* Switching between threads in the same process is often cheaper than switching between processes, although the exact cost depends on the operating system and workload.
* Threads also make sharing in-process data and resources straightforward because they use the same address space.

### Challenges with Multithreading

- Threads can share state directly, which makes communication efficient but requires care when that state is mutable.
- Data consistency is a primary concern. Unsynchronized conflicting accesses can cause data races, while timing-dependent logic can cause broader race conditions.
- Thread creation, context switching, synchronization, and cache contention all add overhead. Poorly managed concurrency can erase the expected performance gains.
- Shared memory must be coordinated with appropriate synchronization, such as mutexes, atomics, semaphores, condition variables, or higher-level concurrent data structures.
- Operating-system scheduling is nondeterministic, which makes timing-dependent bugs difficult to reproduce and test.
- Thread safety often trades some concurrency for correctness. The goal is to synchronize only the state that actually needs protection and keep critical sections small.

#### Data Race

- A **race condition** is a broader logic problem in which correctness depends on the timing or ordering of concurrent operations.
- A **data race** is more specific: two threads access the same memory location concurrently, at least one access is a write, and the accesses are not properly synchronized. In C++, a data race results in undefined behavior.
- Because thread scheduling can occur at many points, code must not rely on an assumed execution order unless that order is enforced explicitly.

Consider two functions, `funA()` and `funB()`, where `funB()` depends on work completed by `funA()`. In a single-threaded program:

```python
funA()
funB()
```

The order is guaranteed. However, in a multithreaded scenario:

```python
# Thread 1
funA()

# Thread 2
funB()
```

The execution order is no longer guaranteed. If `funB()` runs before `funA()` completes, the program may produce an incorrect result. This is an ordering race; it becomes a data race only if the functions perform unsynchronized conflicting accesses to the same memory.

- A data race occurs when threads perform conflicting accesses to the same memory without the synchronization required by the language and memory model.
- Locks, atomics, and other synchronization primitives can establish safe access rules and the ordering relationships needed for correct shared-state updates.

Analogy:  

*Imagine several chefs working on the same dish. Sharing the kitchen is fine, but if two chefs change the same ingredient at the same time without coordinating, the result becomes unpredictable. Threads have the same problem when they perform conflicting accesses to shared state without synchronization.*

Example:

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

// Shared counter variable
int counter = 0;

// Function to increment the counter
void incrementCounter(int numIncrements) {
    for (int i = 0; i < numIncrements; ++i) {
        // Read, increment, and write back the counter
        // This is not an atomic operation and causes a data race
        counter++;
    }
}

int main() {
    const int numThreads = 10;                  // Number of threads
    const int incrementsPerThread = 100000;     // Increments per thread

    std::vector<std::thread> threads;

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Create and start threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(incrementCounter, incrementsPerThread);
    }

    // Wait for all threads to finish
    for (auto& th : threads) {
        th.join();
    }

    // Stop timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // Expected result
    int expected = numThreads * incrementsPerThread;

    // Output results
    std::cout << "Final counter value: " << counter << std::endl;
    std::cout << "Expected counter value: " << expected << std::endl;
    std::cout << "Time taken: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}

```

Possible Output:

```
Final counter value: 282345
Expected counter value: 1000000
Time taken: 0.023456 seconds

```

What is happening:

```
+----------------------------+

| Shared Counter: 100        |

+----------------------------+
        ^            ^

        |            |

  +-----+-----+  +---+------+

  | Thread 1  |  | Thread 2 |

  +-----------+  +----------+

        |               |
        |               |
        |               |

[Thread 1]           [Thread 2]
Read Counter = 100   Read Counter = 100

        |               |
        |               |
        |               |

[Thread 1]           [Thread 2]
Increment: 100 + 1 = 101

        |               |
        |               |
        |               |

[Thread 1]           [Thread 2]
Write Counter = 101  Write Counter = 101

        |               |

+----------------------------+

| Shared Counter: 101        |

+----------------------------+

```

This diagram illustrates one possible lost-update interleaving: both threads read `100`, both compute `101`, and both write `101`. In the C++ program above, however, the unsynchronized accesses to `counter` constitute a data race, so the language does not guarantee any particular outcome.

What do we mean by a resource?

In computing, a resource is any hardware or software asset that a program needs, such as CPU time, memory, storage, network bandwidth, files, locks, or shared data structures. Some resources are limited or require coordinated access. Managing them well helps prevent bottlenecks, excessive contention, and deadlocks.

#### Mutex

- A **mutex** (short for *mutual exclusion*) provides exclusive ownership of a lock, allowing one thread at a time to enter the protected critical section.
- If a thread already owns the mutex, another thread that tries to lock it must wait until the mutex is released.

Analogy:  

*Think of a single-stall restroom with a door lock. One person holds the lock while using the room; everyone else waits until it is released. A mutex provides the same kind of exclusive access for a critical section.*

Example:

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

// Shared counter variable
int counter = 0;

// Mutex to protect the counter
std::mutex counterMutex;

// Function to increment the counter with synchronization
void incrementCounterSafe(int numIncrements) {
    for (int i = 0; i < numIncrements; ++i) {
        std::lock_guard<std::mutex> lock(counterMutex);
        counter++;
    }
}

int main() {
    const int numThreads = 10;
    const int incrementsPerThread = 100000;

    std::vector<std::thread> threads;

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Create and start threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(incrementCounterSafe, incrementsPerThread);
    }

    // Wait for all threads to finish
    for (auto& th : threads) {
        th.join();
    }

    // Stop timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // Expected result
    int expected = numThreads * incrementsPerThread;

    // Output results
    std::cout << "Final counter value: " << counter << std::endl;
    std::cout << "Expected counter value: " << expected << std::endl;
    std::cout << "Time taken: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
```

Possible Output:

```
Final counter value: 1000000
Expected counter value: 1000000
Time taken: 0.234567 seconds
```

What is happening:

```
┌────────────────────────────┐
│      Shared Counter: 100   │
└────────────────────────────┘
           ▲                  ▲
           │                  │
     ┌─────┴─────┐      ┌─────┴─────┐
     │  Thread 1 │      │  Thread 2 │
     └─────┬─────┘      └─────┬─────┘
           │                  │         WAITING 
           │                  -----------------
           ▼                                  |
┌─────────────────────────────────┐           |
│ [Thread 1 acquires mutex]       │           |
│ [Thread 1] Read Counter = 100   │           |
│ [Thread 1] Increment to 101     │           |
│ [Thread 1] Write Counter = 101  │           |
│ [Thread 1 releases mutex]       │           |
└─────────────────────────────────┘           |
                                              ▼ 
                        ┌────────────────────────────────┐
                        │ [Thread 2 acquires mutex]      │
                        │ [Thread 2] Read Counter = 101  │
                        │ [Thread 2] Increment to 102    │
                        │ [Thread 2] Write Counter = 102 │
                        │ [Thread 2 releases mutex]      │
                        └────────────────────────────────┘

```

The mutex ensures that only one thread can modify the shared counter at a time, resulting in a correct final value but with additional locking overhead.

#### Atomic

An **atomic** operation is indivisible with respect to other atomic operations on the same object: other threads do not observe a partially completed update. This makes atomics useful for simple shared state such as counters and flags without requiring a mutex around each operation. In C++, `std::atomic<T>` can be used with supported trivially copyable types, although an atomic type is not necessarily lock-free on every platform.

C++ provides atomic operations through `std::atomic<T>` and types such as `std::atomic_flag`. Operations including `load`, `store`, `fetch_add`, `fetch_sub`, and `compare_exchange` are atomic on the atomic object. Their memory-order argument determines how those operations synchronize with other memory accesses.

What do we gain by using atomics?

- Atomics can be cheaper than mutexes for simple operations and are often implemented with dedicated hardware instructions.
- They avoid mutex ownership and blocking lock acquisition, but they can still suffer from contention, especially when many threads update the same cache line.
- Atomics are convenient for basic shared state such as counters and flags when a single atomic operation expresses the required update.

What do we lose by using atomics?

- Atomics require careful reasoning about memory ordering when synchronization extends beyond the atomic variable itself.
- A single atomic variable is easy to use, but coordinating multi-step invariants or complex data structures is much harder.
- Lock-free algorithms can introduce subtle problems such as livelock and the ABA problem.
- Under heavy contention, atomics may perform no better than lock-based approaches and can sometimes perform worse.

Analogy:

*Think of a vending machine that accepts a payment and updates its internal balance as one indivisible transaction. Other customers never observe the balance halfway through that update. An atomic operation provides a similar all-or-nothing update for one shared value.*

Example:

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

std::atomic<int> counter(0);

void incrementCounterAtomic(int numIncrements) {
    for (int i = 0; i < numIncrements; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    const int numThreads = 10;
    const int incrementsPerThread = 100000;

    std::vector<std::thread> threads;
    
    // Create and start threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(incrementCounterAtomic, incrementsPerThread);
    }
    
    // Wait for all threads to finish
    for (auto& th : threads) {
        th.join();
    }
    
    std::cout << "Final counter value: " << counter << std::endl;
    std::cout << "Expected counter value: " << (numThreads * incrementsPerThread) << std::endl;
    return 0;
}
```

What is happening:

```
             Atomic Counter
    Thread 1       |         Thread 2
-------------------+-------------------
  Read & Inc        | 
      |            Read & Inc
      |                |
  Write: 101 ----> No Interruption <---- Write: 102
      |                |
      v                v
  next iteration  next iteration

  (All increments happen as atomic steps,
   so partial updates are never seen)
```

To clear up the common confusion surrounding this term, let’s clarify how it differs from related concepts:

- **Atomic** describes an operation that is indivisible with respect to competing accesses to the same atomic object. It prevents data races on that atomic object, but it does not make a larger algorithm automatically free of race conditions.
- **Lock-free** means the system as a whole is guaranteed to make progress: at least one thread completes an operation in a finite number of steps, even if individual threads may starve.
- **Wait-free** is stronger: every participating thread is guaranteed to complete its operation within a bounded number of steps.

#### Deadlock

A **deadlock** occurs when two or more threads are blocked, each waiting for a lock that another thread already holds. Because all threads are waiting on one another, no progress can be made, and the system is effectively stuck.

Analogy:  

*Imagine two cars on a narrow one-lane bridge coming from opposite ends. Each driver refuses to back up, and neither can move forward. Both are blocked indefinitely, waiting for the other to yield.*

Example:

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex mutexA;
std::mutex mutexB;

void threadFunc1() {
    std::lock_guard<std::mutex> lock1(mutexA);
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::lock_guard<std::mutex> lock2(mutexB);
}

void threadFunc2() {
    std::lock_guard<std::mutex> lock1(mutexB);
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // simulate work
    std::lock_guard<std::mutex> lock2(mutexA);
}

int main() {
    std::thread t1(threadFunc1);
    std::thread t2(threadFunc2);

    t1.join();
    t2.join();

    return 0;
}
```

What is happening:

```
Thread 1                    Thread 2
    |                           |
    v                           v
 Lock(mutexA)              Lock(mutexB)
      |                         |
      |-------Wait(mutexB) <----|
      |                         |
      |                         |-------Wait(mutexA)
      v                         v
   BLOCKED                   BLOCKED

(Each thread holds one lock and waits
for the other lock to be released.
Neither lock is ever freed -> deadlock)
```

#### Livelock

A **livelock** occurs when threads keep reacting to one another but still fail to make useful progress. Unlike a deadlock, the threads are active rather than blocked; they repeatedly change state or retry without completing the work.

Analogy:  

*Picture two people in a narrow hallway who both step aside to let the other pass—only to keep stepping in the same direction repeatedly. They’re not standing still, but neither can get by the other.*

Example:

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

std::mutex mutex1;
std::mutex mutex2;
std::atomic<bool> is_done(false);

void thread1() {
    while (!is_done.load()) {
        if (mutex1.try_lock()) {
            if (mutex2.try_lock()) {
                std::cout << "Thread 1 completes work.\n";
                is_done.store(true);
                mutex2.unlock();
            }
            mutex1.unlock();
        }
        // Thread tries, fails or succeeds,
        // then repeats without blocking indefinitely.
    }
}

void thread2() {
    while (!is_done.load()) {
        if (mutex2.try_lock()) {
            if (mutex1.try_lock()) {
                std::cout << "Thread 2 completes work.\n";
                is_done.store(true);
                mutex1.unlock();
            }
            mutex2.unlock();
        }
    }
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);

    t1.join();
    t2.join();

    return 0;
}
```

What is happening:

```
Thread 1                Thread 2
  try_lock(mutex1)       try_lock(mutex2)
       |                      |
   success?               success?
       |                      |
   try_lock(mutex2)       try_lock(mutex1)
       |                      |
   success?               success?
       |                      |
 release/retry         release/retry
       |                      |
       v                      v
  loop again             loop again

(Under an unlucky repeated interleaving, both threads can
keep acquiring one lock, failing on the other, releasing,
and retrying in step. They remain active but make no
forward progress: a livelock.)
```
 
#### Semaphore

A **semaphore** is a synchronization primitive that uses a counter to limit how many threads may proceed at once. An `acquire` operation waits until the count is positive and then decrements it atomically. A `release` operation increments the count and may wake a waiting thread.

Analogy:  

*Think of a parking garage with a limited number of spaces. Each car (thread) must check if a space is available before entering (acquire). If no space is free, the car must wait. When a car leaves (release), a space opens up for the next waiting car.*

Example (using C++20 counting semaphore):

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>
#include <chrono>

// A counting semaphore initialized to allow 2 concurrent threads
std::counting_semaphore<2> sem(2);

void worker(int id) {
    // Acquire a slot
    sem.acquire();
    std::cout << "Thread " << id << " enters critical section.\n";
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Thread " << id << " leaves critical section.\n";
    // Release the slot
    sem.release();
}

int main() {
    std::vector<std::thread> threads;
    
    // Launch multiple threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(worker, i);
    }
    
    // Wait for all to finish
    for (auto &t : threads) {
        t.join();
    }
    
    return 0;
}
```

What is happening:

```
               [Semaphore with count = 2]
 -----------------+-----------------+-----------------
  Thread 0        |   Thread 1     |    Thread 2 ...
  tries sem.acquire()              | 
        |                          |
[Slot1 free, Slot2 free]          |
  acquires Slot1 -> count=1       | 
        |                          |
        v                          |
   "In critical section"           |
        |                          |
  Thread 1 tries sem.acquire()     |
  acquires Slot2 -> count=0        |
        |                          |
        v                          |
   "In critical section"           |
                   ... Meanwhile ...
               Thread 2 tries sem.acquire()
                     |       
                     v
               Must wait because count=0
               
    Once Thread 0 or 1 calls sem.release():
    - count increments by 1
    - Thread 2 (or next in line) acquires and enters
```

#### Common Misconceptions

Binary Semaphore vs. Mutex  

There is a common misconception that a binary semaphore and a mutex are equivalent. While both can restrict access to a resource, their primary use cases differ:

- A **mutex** represents exclusive ownership. The thread that locks it is responsible for unlocking it.
- A **binary semaphore** has a count of 0 or 1 and does not have mutex-style ownership. This makes it useful for signaling: one thread can release the semaphore to indicate that an event or resource is available, and a waiting thread can acquire it.

Multithreading Automatically Improves Performance

Many developers believe that incorporating multiple threads always leads to faster execution. However, multithreading can also slow down an application if not designed and tuned properly. The overhead of context switching, synchronization, and resource contention can negate performance gains, especially if the tasks are not well-suited for parallelism.

More Threads Equals Better Performance

It is often assumed that creating more threads will consistently boost performance. In reality, once the number of threads exceeds the available CPU cores or the nature of the task’s concurrency limits, performance may degrade. Excessive thread creation can lead to increased scheduling overhead, cache thrashing, and resource contention—ultimately harming efficiency.

Multithreaded Code Is Always Harder to Write and Maintain  

While concurrency introduces challenges—such as synchronization, potential race conditions, and timing-related bugs—multithreaded code is not necessarily more difficult to manage than single-threaded code. Modern languages and frameworks provide abstractions (e.g., thread pools, futures, async/await mechanisms) that simplify parallelism. With proper design, testing strategies, and usage of these tools, writing reliable and maintainable multithreaded applications becomes more approachable.

#### Problems for which multithreading is the answer

- CPU-intensive computations that can be divided into independent work and can actually run in parallel in the chosen language/runtime.
- Servers that must handle many independent client requests or connections concurrently.
- Tasks that can be intuitively split into independent processing steps, allowing different threads to handle separate parts of a workflow concurrently.
- Continuous access to a large read-only data set, where multiple threads can efficiently read and process the data without needing to modify it.
- Tasks whose performance would be unacceptable as a single thread, necessitating parallel execution to meet performance requirements.
- Managing concurrent access to multiple resources, such as an operating system coordinating access to hardware components, memory, and peripherals.
- Processing a stream of large data files, enabling different threads to handle different segments of the data simultaneously for faster processing.
- Problems where each step has a clear input and output, facilitating parallel processing of sequential steps in a pipeline.
- Processes where the workload cannot be anticipated, allowing the system to dynamically allocate threads to handle varying loads effectively.
- Real-time data processing tasks, such as financial trading systems that require immediate handling of incoming market data to execute trades without delay.
- Blocking I/O workloads where multiple threads can overlap waits for files, networks, or other external resources.
- Maintaining user interface responsiveness in applications by offloading long-running tasks to background threads, ensuring the UI remains interactive.
- Parallel data processing pipelines, like ETL (Extract, Transform, Load) processes in data warehousing, where different stages run concurrently to enhance throughput.
- Simulation and modeling applications, such as climate models or physics simulations, that divide the environment into regions processed in parallel to speed up computations.
- Network servers and web services that handle multiple client connections simultaneously, with each thread managing a separate client session to ensure efficient request handling.
- Machine learning model training, where large neural networks are trained by parallelizing computations across multiple threads or cores to accelerate the learning process.
- Multimedia processing tasks, including audio and video encoding or decoding, where different streams or segments are processed in parallel to reduce latency and improve performance.
- Automated testing and continuous integration systems that run multiple test suites or build processes in parallel to speed up the development and deployment pipeline.

#### Problems for which multithreading is not the answer

- Tasks with sequential dependencies require operations to be performed in a strict order without the possibility of parallel execution, making multithreading ineffective.
- Minimal processing tasks involve simple or quick operations where the overhead of creating and managing threads outweighs any potential performance gains.
- High contention for shared resources occurs when multiple threads frequently compete for the same resources, leading to excessive locking and reduced performance.
- Applications relying on single-threaded libraries or APIs are not designed to be thread-safe, making multithreading difficult or error-prone.
- Limited hardware resources mean environments have insufficient CPU cores or memory, where adding more threads could degrade overall system performance.
- Systems with strict timing guarantees may not benefit from ordinary, unmanaged multithreading because general-purpose scheduling can introduce unpredictable delays. Real-time systems can still use threads when paired with appropriate real-time scheduling and design.
- Applications requiring high synchronization involve tasks that need extensive coordination between threads, resulting in bottlenecks and diminishing returns from parallelism.
- Debugging and maintenance complexity arises in projects where the added complexity of multithreading introduces significant challenges in debugging, testing, and maintaining the codebase.
- Deterministic execution needs are present in applications that require consistent and repeatable behavior for debugging, security, or compliance reasons, which can be disrupted by the non-deterministic nature of multithreading.
- Environments with poor multithreading support have programming languages or runtime environments that lack robust multithreading capabilities, making implementation difficult or inefficient.
- Tasks better suited for asynchronous programming benefit from asynchronous, non-blocking approaches that provide better performance and scalability without the complexities of multithreading.
- Problems better addressed by multiprocessing or distributed computing involve situations where using multiple processes or distributing tasks across different machines is more effective than using multiple threads within a single process.
- Memory-constrained applications need to minimize memory usage, as each thread consumes additional memory for its stack and management overhead.
- Simple, single-user applications operate with a single user or do not require concurrent processing, where multithreading offers no tangible benefits.
- Tasks with high initialization costs involve operations where the cost of starting and stopping threads is prohibitively high compared to the task's execution time.
- Security-sensitive applications could introduce vulnerabilities through race conditions or improper handling of shared data when using multithreading.
- Legacy codebases are existing applications that were not designed with multithreading in mind, where retrofitting multithreading could be risky or impractical.
- Energy-constrained devices are battery-powered or low-energy devices where the additional power consumption from managing multiple threads is a concern.

#### Typical Applications

| Use case | Pattern summary | C++ (preferred) | Python (preferred) | Why this choice | Shutdown behavior |
|---|---|---|---|---|---|
| High-throughput web server: handle many requests | Worker pool reuses threads for each request | Thread pool (joinable) | ThreadPoolExecutor (joinable) | Limits thread count, good control of lifecycle, back-pressure via queue | Stop accepting work, drain queue, join pool |
| Background logging/telemetry uploader | Background worker reads from a queue | jthread (auto-join) | Managed thread or daemon thread | A managed worker supports cancellation and flushing; daemon threads are suitable only for best-effort work | Signal stop and flush if delivery matters; daemon work may be dropped at exit |
| Periodic metrics/health pinger | Loop with sleep to send heartbeats | jthread (auto-join) | Daemon thread | Runs for app lifetime; easy cancel/stop; ok if it ends abruptly | Signal stop, allow one last send if needed |
| GUI app: offload long task (keep UI responsive) | Worker thread performs blocking work, reports progress | Joinable thread (or QThread) | Joinable thread | Ensure task finishes/cleans up before closing app | Signal cancel, join before window closes |
| Real-time sensor acquisition → queue | Reader thread pushes samples to a bounded queue | Joinable thread (or jthread) | Joinable thread | Data loss unacceptable; deterministic shutdown | Stop signal, flush buffer, join |
| Producer→consumer pipeline (download→parse→write) | Stage per thread, connected by queues | Joinable threads or pool | Joinable threads with queue | Back-pressure and orderly teardown | Send sentinels, join in stage order |
| Parallel I/O (e.g., web scraping many hosts) | Cap concurrency using pool/executor | Thread pool (joinable) | ThreadPoolExecutor (joinable) | I/O-bound; pooling avoids oversubscription | Shutdown executor, wait=True |
| CPU-bound parallel compute (e.g., image filters) | Parallelize independent CPU work | Thread pool (joinable) | Usually processes for GIL-enabled CPython | C++ threads can run in parallel; the GIL limits CPU-bound Python threads in conventional CPython builds | Join pool / close process pool |
| Connection timeout watchdog | Sleeps, then cancels/alerts if overdue | jthread (auto-join) | Daemon thread or Timer | Short-lived helper; safe if it dies at exit | Cancel timer / stop token |
| Cache warmer / prefetcher | Preloads likely-needed data in background | jthread or managed pool | Daemon thread or executor | Best-effort work can run in the background, but keeping lifecycle control avoids unsafe detached access | Cancel or stop cleanly; best-effort work may be skipped at shutdown |
| Background email/SMS sender in a web app | Queue of messages consumed by workers | Thread pool (joinable) | ThreadPoolExecutor (joinable) | Must ensure delivery or retry logic | Drain queue, join; otherwise use external task queue |
| Game asset streaming loader | Loads textures/models while game runs | Joinable thread | Joinable thread | Coordinate with main loop; avoid torn state | Signal cancel, join before scene swap |
| Game telemetry/analytics uploader | Buffers and uploads non-critical events | jthread or managed worker | Daemon thread or managed worker | Best-effort work should not stall the main loop, but managed lifetime avoids unsafe detached access | Best-effort flush; stop managed workers cleanly |
| File system watcher (hot-reload) | Watches dirs and enqueues change events | Joinable thread | Joinable thread | Needs clean shutdown to release handles | Stop watcher, join |
| DB connection keepalive / pool maintenance | Occasional pings, cleanup of idle conns | jthread (auto-join) | Daemon thread | Low-importance periodic task | Cancel on stop; ok to skip final ping |
| CLI tool runs background maintenance (e.g., log rotation) | Helper runs concurrently with the main work | jthread or joinable thread | Managed thread | A thread cannot outlive its process, so important maintenance should be completed or handed to a separate service/process | Signal stop and join, or delegate persistent work to another process |
| Small chat server: one thread per client (naïve) | Spawn per connection, handle then exit | Joinable thread or pool | Joinable thread or executor | Simple at small scale, but explicit lifecycle management is safer than detaching | Prefer a pool for scale; stop accepting clients and join workers on shutdown |
| Market data listener → queue | Network read loop pushes updates | Joinable thread (or jthread) | Joinable thread | Data integrity and ordering matter | Signal stop, flush, join |

### Examples

#### Examples in C++

In a typical C++ program, `main()` runs on the initial thread. That thread can create additional threads so work can proceed concurrently. Since C++11, the Standard Library has provided `std::thread` for creating and managing threads. A new thread is started by passing a callable, along with any arguments, to the `std::thread` constructor.

##### Creating Threads

A new C++ thread is created by constructing a `std::thread`. Its constructor accepts a callable—such as a function, lambda, or function object—plus any arguments to pass to that callable.

```cpp
#include <iostream>
#include <thread>
#include <string>

void printMessage(const std::string& message) {
    std::cout << message << std::endl;
}

int main() {
    std::thread t1(printMessage, "Hello from thread!");
    t1.join(); // Wait for the thread to finish
    return 0;
}
```

In this example, `printMessage` is called in a separate thread, and the main thread waits for `t1` to complete using `join()`.

##### Thread Joining

The `join()` function is called on a `std::thread` object to wait for the associated thread to complete execution. This blocks the calling thread until the thread represented by `std::thread` finishes.

Advantages:

- The calling thread can wait for a worker to complete before using its results or continuing shutdown.
- Joining provides a clear lifecycle boundary and helps prevent threads from being abandoned while they still use process resources.

Disadvantages:
 
- `join()` blocks the calling thread until the target thread finishes, so joining at the wrong time can hurt responsiveness.
- A join can wait indefinitely if the target thread is stuck, so shutdown and cancellation paths still need careful design.

```cpp
t1.join(); // Main thread waits for t1 to finish
```

##### Thread Detaching

Using `detach()`, a thread is disassociated from its `std::thread` object and continues independently within the process. The original thread can continue without waiting for it. Once detached, the thread cannot be joined through that `std::thread` object, which makes lifetime and shutdown management much harder.

Advantages:

- The creating thread can continue without waiting for the detached thread.
- It can be acceptable for narrowly scoped, truly independent best-effort work.

Disadvantages:

- The creating code loses the ability to join the thread or directly manage its completion.
- The process may exit while the thread is still running, and detached threads can easily outlive objects they reference if lifetimes are not designed carefully.

```cpp
std::thread t2(printMessage, "This is a detached thread");
t2.detach(); // Main thread does not wait for t2
```

##### Thread Lifecycle and Resource Management

Each thread has a lifecycle, beginning with creation, execution, and finally termination. Upon termination, the resources held by the thread need to be cleaned up. If a thread object goes out of scope and is still joinable (not yet joined or detached), the program will terminate with `std::terminate` because it is considered an error to destroy a `std::thread` object without properly handling the thread.

##### Passing Arguments to Threads

Arguments can be passed to the thread function through the `std::thread` constructor. The arguments are copied or moved as necessary. Special care must be taken when passing pointers or references, as these must refer to objects that remain valid throughout the thread's execution.

```cpp
#include <iostream>
#include <thread>

void printSum(int a, int b) {
    std::cout << "Sum: " << (a + b) << std::endl;
}

int main() {
    int x = 5, y = 10;
    std::thread t(printSum, x, y); // Passing arguments by value
    t.join();
    return 0;
}
```

In this example, `x` and `y` are passed by value to the `printSum` function.

##### Using Lambdas with Threads

Lambda expressions provide a convenient way to define thread tasks inline. They can capture local variables by value or reference, allowing for flexible and concise thread management.

```cpp
#include <iostream>
#include <thread>

int main() {
    int a = 5, b = 10;
    std::thread t([a, b]() {
        std::cout << "Lambda Sum: " << (a + b) << std::endl;
    });
    t.join();
    return 0;
}
```

In this case, the lambda captures `a` and `b` by value and uses them inside the thread.

##### Mutex for Synchronization

`std::mutex` is used to protect shared data from being accessed simultaneously by multiple threads. It ensures that only one thread can access the critical section at a time, preventing data races.

```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int sharedCounter = 0;

void increment() {
    std::lock_guard<std::mutex> lock(mtx);
    ++sharedCounter;
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Shared Counter: " << sharedCounter << std::endl;
    return 0;
}
```

In this example, `std::lock_guard` automatically locks the mutex on creation and unlocks it on destruction, ensuring the increment operation is thread-safe.

##### Deadlocks and Avoidance

Deadlocks occur when threads wait on one another in a cycle, so none can proceed. Common prevention techniques include acquiring locks in a consistent order, reducing nested locking, or using utilities such as `std::lock`/`std::scoped_lock` that acquire multiple mutexes without deadlocking.

```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex1;
std::mutex mutex2;

void taskA() {
    std::lock(mutex1, mutex2);
    std::lock_guard<std::mutex> lock1(mutex1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mutex2, std::adopt_lock);
    std::cout << "Task A acquired both mutexes\n";
}

void taskB() {
    std::lock(mutex1, mutex2);
    std::lock_guard<std::mutex> lock1(mutex1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mutex2, std::adopt_lock);
    std::cout << "Task B acquired both mutexes\n";
}

int main() {
    std::thread t1(taskA);
    std::thread t2(taskB);
    t1.join();
    t2.join();
    return 0;
}
```

Here, `std::lock` uses a deadlock-avoidance algorithm to acquire both mutexes. The `std::lock_guard` objects then adopt ownership so the mutexes are released automatically when the function returns.

##### Condition Variables

`std::condition_variable` is used for thread synchronization by allowing threads to wait until they are notified to proceed. This is useful for scenarios where a thread must wait for some condition to become true.

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void print_id(int id) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; });
    std::cout << "Thread " << id << "\n";
}

void set_ready() {
    std::unique_lock<std::mutex> lock(mtx);
    ready = true;
    cv.notify_all();
}

int main() {
    std::thread t1(print_id, 1);
    std::thread t2(print_id, 2);
    std::thread t3(set_ready);

    t1.join();
    t2.join();
    t3.join();
    return 0;
}
```

In this example, `cv.wait` makes the threads wait until `ready` becomes true. `set_ready` changes the condition and notifies all waiting threads.

##### Semaphores

C++20 introduces `std::counting_semaphore` and `std::binary_semaphore`. Semaphores are synchronization primitives that control access to a common resource by multiple threads. They use a counter to allow a fixed number of threads to access a resource concurrently.

```cpp
#include <iostream>
#include <thread>
#include <semaphore>
#include <chrono>

std::binary_semaphore semaphore(1);

void task(int id) {
    semaphore.acquire();
    std::cout << "Task " << id << " is running\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate some work
    semaphore.release();
}

int main() {
    std::thread t1(task, 1);
    std::thread t2(task, 2);
    t1.join();
    t2.join();
    return 0;
}
```

Here, `semaphore.acquire()` ensures that only one thread can access the critical section at a time, and `semaphore.release()` signals that the resource is available again.

##### Thread Local Storage

C++ provides thread-local storage via the `thread_local` keyword, allowing data to be local to each thread. This is useful when each thread requires its own instance of a variable, such as when storing non-shared data.

```cpp
#include <iostream>
#include <thread>

thread_local int localVar = 0;

void increment(int id) {
    ++localVar;
    std::cout << "Thread " << id << ": localVar = " << localVar << std::endl;
}

int main() {
    std::thread t1(increment, 1);
    std::thread t2(increment, 2);
    t1.join();


    t2.join();
    return 0;
}
```

In this example, each thread has its own instance of `localVar`, independent of the other threads.

##### Atomic Operations

For simple shared values that need atomic updates, C++ provides the `std::atomic` template. Atomics can avoid an explicit mutex for operations such as counters and flags, but `std::atomic<T>` is not guaranteed to be lock-free on every platform.

```cpp
#include <iostream>
#include <thread>
#include <atomic>

std::atomic<int> atomicCounter(0);

void increment() {
    for (int i = 0; i < 100000; ++i) {
        ++atomicCounter;
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Atomic Counter: " << atomicCounter << std::endl;
    return 0;
}
```

In this example, `std::atomic<int>` ensures that the increment operation is atomic, preventing data races.

##### Memory Orderings

When using atomic operations in C++, we not only specify *which* operations should be atomic, but also *how* they synchronize with other memory operations in the program. This “how” is controlled by **memory orderings**—a set of rules that govern visibility and ordering of reads and writes.

C++ provides six memory order enumerations in `std::memory_order`:

1. **`std::memory_order_relaxed`**  
2. **`std::memory_order_consume`** (mostly unimplemented in mainstream compilers)  
3. **`std::memory_order_acquire`**  
4. **`std::memory_order_release`**  
5. **`std::memory_order_acq_rel`**  
6. **`std::memory_order_seq_cst`**  

Each ordering provides different guarantees about how an atomic operation relates to other memory accesses. Choosing the correct ordering affects correctness first and can also affect performance.

Below is a comparison table of the main C++ memory orderings, their guarantees, common uses, and common pitfalls.

| **Memory Order**               | **Brief Description**                                                          | **Guarantees**                                                                                                                                                                                          | **Common Use Cases**                                                                                                                    | **Pitfalls & Advice**                                                                                                                                        |
|--------------------------------|-------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------|
| **`std::memory_order_relaxed`** | Provides atomicity without synchronization ordering for other memory accesses | - The atomic operation itself is indivisible <br/> - Operations on the same atomic still follow its modification order, but there is no cross-thread synchronization for unrelated memory | - Counters or statistics where only the atomic value matters <br/> - Flags that do not publish other data | - Easy to choose incorrectly when other state depends on the atomic update <br/> - Use only when no ordering relationship is required |
| **`std::memory_order_consume`** | Intended to enforce data dependency ordering (rarely implemented properly)   | - Theoretically only dependent reads are ordered  <br/> - In practice, compilers often treat it like `acquire`                                                                                              | - Very specialized, mostly replaced by `acquire` in real-world code                                                                                                           | - Not well supported by most compilers <br/> - Avoid in portable or production code                                                                                              |
| **`std::memory_order_acquire`** | Prevents later operations from being reordered before the acquire             | - When an acquire reads a value from a matching release sequence, earlier writes from the releasing thread become visible to the acquiring thread <br/> - Acts as a one-way ordering barrier after the acquire | - Loading a “ready” flag before reading published data <br/> - Consumer-side synchronization | - An acquire only synchronizes when it observes the relevant release/release sequence <br/> - Usually paired with `release` in producer-consumer patterns |
| **`std::memory_order_release`** | Prevents earlier operations from being reordered after the release             | - Publishes prior writes to a thread whose matching acquire observes this release (or its release sequence) <br/> - Acts as a one-way ordering barrier before the release | - Setting a “ready” flag after preparing shared data <br/> - Producer-side synchronization | - A release alone does not make another thread observe the data <br/> - It must participate in a matching synchronization relationship |
| **`std::memory_order_acq_rel`** | Acquire + Release in one read-modify-write operation                          | - Combines the effects of `acquire` and `release` for RMW ops (e.g., `fetch_add`, `compare_exchange`) <br/> - Ensures no reorder before or after the operation                                              | - Updating shared state in a single atomic step where you must see previous writes and publish new writes (e.g., lock-free structures)                                     | - Can be stronger (thus slower) than needed if you only require a one-way barrier <br/> - Must be used carefully in highly concurrent scenarios              |
| **`std::memory_order_seq_cst`** | Enforces total sequential consistency across all threads                     | - Provides a single, global order of all sequentially consistent operations <br/> - Easiest model to reason about, strongest ordering guarantee                                                             | - When correctness is paramount and performance concerns are secondary <br/> - Prototyping concurrency code before optimizing                                               | - Highest potential performance cost <br/> - May introduce unnecessary fences on weaker architectures                                                        |

What Do We Gain By Careful Use of Memory Orderings?

- **Weaker orderings** such as `relaxed`, `acquire`, and `release` can compile to more efficient instructions on some hardware, resulting in better performance compared to using a blanket `seq_cst`.
- Careful use of memory orderings provides **control** by ensuring only the minimal necessary barriers are in place, which helps prevent the use of expensive hardware fences when they are not needed.

What Do We Lose / Need to Beware Of?

- Managing memory orderings introduces **complexity**, making it easy to introduce subtle bugs if the chosen ordering is too weak to guarantee the necessary data visibility.
- Code that utilizes specialized memory orderings can suffer from reduced **portability** and become harder to maintain, especially when new developers join the project.
- Using an ordering stronger than necessary, such as `seq_cst` everywhere, can limit compiler or hardware optimizations on some architectures.

Analogy

Imagine you’re coordinating a relay race: 

- A `release` operation is like handing the baton off—ensuring everything you’ve done (run your segment) is finished before the next runner picks it up.  
- An `acquire` operation is the next runner receiving the baton—ensuring they see everything you did (how far you ran, the state of the race) the moment they take it.  
- `relaxed` would be like running without caring about handing the baton off or receiving it properly—fast, but not synchronized.  
- `seq_cst` would be like having a strict official track judge making sure everyone runs in a strictly observed, universal order—less chance of cheating but more overhead.

Example

Below is a small snippet that demonstrates `release` and `acquire`:

```cpp
#include <atomic>
#include <vector>
#include <thread>
#include <iostream>

struct SharedData {
    int value;
};

std::atomic<bool> ready(false);
SharedData data;

void producer() {
    // 1. Write to shared data
    data.value = 42;

    // 2. Publish that data is ready
    ready.store(true, std::memory_order_release);
}

void consumer() {
    // Wait until the data is ready
    while (!ready.load(std::memory_order_acquire)) {
        // spin or sleep
    }

    // Now it is guaranteed that we see data.value = 42
    std::cout << "Shared data value = " << data.value << std::endl;
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
    return 0;
}
```

- The **producer** writes `data.value = 42` and then calls `ready.store(true, std::memory_order_release)`, ensuring that any subsequent acquire operation on `ready` will see the updated `data.value`.
- The **consumer** spins until `ready.load(std::memory_order_acquire)` becomes true, and because it’s an acquire load, once it returns true, the consumer also sees `data.value = 42`.

What is happening:

```
   Producer Thread                Consumer Thread
         |                              |
   data.value = 42                     ...
         |                              |
 ready.store(true, release)     ready.load(acquire) --> sees true
         |                              |
         v                              |
    [ memory fence ]                    v
                                 sees data.value = 42
```

- The **release** store publishes earlier writes, including `data.value = 42`.
- When the **acquire** load reads the value written by that release, the operations synchronize, so the consumer can safely observe `data.value = 42`.

##### Performance Considerations and Best Practices

- The frequent creation and destruction of threads can be costly, leading to significant overhead. To minimize this, it is advisable to use thread pools or reuse threads, which can reduce the performance impact associated with thread lifecycle management.
- Synchronization mechanisms, such as mutexes, should be used sparingly because excessive synchronization can lead to contention and reduced performance. It is important to apply these mechanisms only when necessary to avoid unnecessary delays and overhead.
- Protect shared mutable data with synchronization appropriate to the access pattern. That may mean a mutex, an atomic, a reader-writer lock, a concurrent data structure, or another mechanism that establishes safe access and ordering.
- Utilizing modern features introduced in C++11 and later, such as `std::thread`, `std::mutex`, `std::lock_guard`, and `std::future`, can greatly simplify thread management and help avoid common pitfalls. These features provide robust and standardized ways to handle concurrency, making the code more maintainable and less error-prone.

Here are some example code snippets demonstrating various aspects of multithreading in C++:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | [single_worker_thread](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/01_single_worker_thread.cpp)  | Introduce the concept of threads by creating a single worker thread using `std::thread`.         |
| 2  |  [thread_subclass](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/02_thread_subclass.cpp)        | Demonstrate a custom thread wrapper built around `std::thread`.     |
| 3  |  [multiple_worker_threads](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/03_multiple_worker_threads.cpp)  | Show how to create and manage multiple worker threads using `std::thread`.                      |
| 4  |  [race_condition](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/04_race_condition.cpp)        | Explain race conditions and their impact on multi-threaded applications using C++ examples.      |
| 5  |  [mutex](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/05_mutex.cpp)    | Illustrate the use of `std::mutex` to protect shared resources and avoid race conditions in C++ applications. |
| 6  |  [semaphore](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/06_semaphore.cpp)           | Demonstrate the use of `std::counting_semaphore` to limit the number of concurrent threads accessing a shared resource in C++ applications. |
| 7  |  [producer_consumer](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/07_producer_consumer.cpp)    | Present a classic multi-threading problem (Producer-Consumer) and its solution using C++ synchronization mechanisms like `std::mutex` and `std::condition_variable`. |
| 8  |  [fetch_parallel](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/08_fetch_parallel.cpp)        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources using C++ threads. |
| 9  |  [merge_sort](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/09_merge_sort.cpp)           | Use multi-threading in C++ to parallelize a merge sort algorithm, demonstrating the potential for performance improvements. |
| 10 |  [schedule_every_n_sec](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/10_schedule_every_n_sec.cpp)   | Show how to schedule tasks to run periodically at fixed intervals using C++ threads. |
| 11 |  [barrier](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/11_barrier.cpp)          | Demonstrate the use of `std::barrier` to synchronize multiple threads at a specific point in the execution. |
| 12 |  [thread_local_storage](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/12_thread_local_storage.cpp)   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  [thread_pool](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/13_thread_pool.cpp)          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  [reader_writer_lock](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/14_reader_writer_lock.cpp)   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |

#### Examples in Python

Python provides built-in threading support through the `threading` module. In conventional GIL-enabled CPython builds, only one thread executes Python bytecode at a time, so threads usually do not speed up CPU-bound Python code. They remain useful for I/O-bound tasks because one thread can run while another waits for external I/O. Other Python implementations, and free-threaded CPython builds, can have different execution characteristics.

##### Creating Threads

To create a new thread, you can instantiate the `Thread` class from the `threading` module. The target function to be executed by the thread is passed to the `target` parameter, along with any arguments required by the function.

```python
import threading

def print_message(message):
    print(message)

# Create a thread
t1 = threading.Thread(target=print_message, args=("Hello from thread!",))
t1.start()
t1.join()  # Wait for the thread to finish
```

In this example, the `print_message` function is executed in a new thread.

##### Thread Joining

Using the `join()` method ensures that the main thread waits for the completion of the thread. This is important for coordinating threads, especially when the main program depends on the thread's results.

```python
t1.join()  # Main thread waits for t1 to finish
```

##### Thread Detaching

Python threads do not have a `detach()` method like C++. By default, threads are non-daemon threads, and the interpreter waits for them before normal program shutdown. A thread can instead be marked as a daemon, in which case it does not keep the process alive and may be stopped abruptly during shutdown. Important work should therefore use non-daemon threads and an explicit completion or shutdown protocol.

##### Thread Lifecycle and Resource Management

Python manages thread objects and interpreter integration, but application code is still responsible for thread lifecycle. Join threads that must finish, provide stop signals for long-running workers, and avoid leaving important work to daemon threads at shutdown.

##### Passing Arguments to Threads

Arguments can be passed to the thread function via the `args` parameter when creating the `Thread` object. This allows for flexible and dynamic argument passing.

```python
import threading

def add(a, b):
    print(f"Sum: {a + b}")

# Create a thread
t2 = threading.Thread(target=add, args=(5, 10))
t2.start()
t2.join()
```

##### Using Lambdas with Threads

Lambda expressions can also be used with threads, providing a concise way to define thread tasks. This is particularly useful for simple operations.

```python
import threading

# Create a thread with a lambda function
t3 = threading.Thread(target=lambda: print("Hello from a lambda thread"))
t3.start()
t3.join()
```

##### Mutex for Synchronization

The `Lock` class from the `threading` module is used to ensure that only one thread accesses a critical section of code at a time. This prevents race conditions by locking the shared resource.

```python
import threading

counter = 0
counter_lock = threading.Lock()

def increment():
    global counter
    with counter_lock:
        counter += 1

# Create multiple threads
threads = [threading.Thread(target=increment) for _ in range(10)]

for t in threads:
    t.start()

for t in threads:
    t.join()

print(f"Counter: {counter}")
```

In this example, `counter_lock` ensures that only one thread modifies the `counter` variable at a time.

##### Deadlocks and Avoidance

Deadlocks can occur when threads acquire multiple locks in incompatible orders. Common prevention techniques include using a consistent lock order, keeping lock scopes small, or using timed/non-blocking acquisition when appropriate. The example below intentionally uses opposite lock orders and can deadlock.

```python
import threading

lock1 = threading.Lock()
lock2 = threading.Lock()

def task1():
    with lock1:
        print("Task 1 acquired lock1")
        with lock2:
            print("Task 1 acquired lock2")

def task2():
    with lock2:
        print("Task 2 acquired lock2")
        with lock1:
            print("Task 2 acquired lock1")

# Create threads
t4 = threading.Thread(target=task1)
t5 = threading.Thread(target=task2)

t4.start()
t5.start()
t4.join()
t5.join()
```

In this example, `task1` acquires `lock1` then `lock2`, while `task2` does the reverse. If each thread acquires its first lock before the other attempts the second, they can deadlock. Using the same acquisition order in both functions avoids that cycle.

##### Condition Variables

`Condition` variables allow threads to wait for some condition to be true before proceeding. This is useful in producer-consumer scenarios.

```python
import threading

condition = threading.Condition()
item_available = False

def producer():
    global item_available
    with condition:
        item_available = True
        print("Producer produced an item")
        condition.notify()

def consumer():
    global item_available
    with condition:
        condition.wait_for(lambda: item_available)
        print("Consumer consumed an item")
        item_available = False

# Create threads
t6 = threading.Thread(target=producer)
t7 = threading.Thread(target=consumer)

t6.start()
t7.start()
t6.join()
t7.join()
```

Here, the consumer waits for the producer to produce an item before proceeding.

##### Semaphores

Python's `threading` module includes `Semaphore` and `BoundedSemaphore` for managing access to a limited number of resources.

```python
import threading
import time

sem = threading.Semaphore(2)  # Allows up to 2 threads to access the resource

def access_resource(thread_id):
    with sem:
        print(f"Thread {thread_id} is accessing the resource")
        # Simulate some work
        time.sleep(1)

# Create multiple threads
threads = [threading.Thread(target=access_resource, args=(i,)) for i in range(5)]

for t in threads:
    t.start()

for t in threads:
    t.join()
```

In this example, the semaphore limits access to a resource, allowing only two threads to enter the critical section at a time.

##### Thread Local Storage

Python provides `threading.local()` to store data that should not be shared between threads.

```python
import threading

local_data = threading.local()

def process():
    local_data.value = 5
    print(f"Thread {threading.current_thread().name} has value {local_data.value}")

# Create threads
t8 = threading.Thread(target=process, name="Thread-A")
t9 = threading.Thread(target=process, name="Thread-B")

t8.start()
t9.start()
t8.join()
t9.join()
```

In this example, each thread has its own `local_data` value, independent of the others.

##### Atomic Operations

In multithreaded Python code, it is easy to assume that the GIL makes compound operations such as `counter += 1` safe. That is not a portable synchronization guarantee. Code that shares mutable state should use an explicit synchronization mechanism rather than depend on interpreter implementation details.

In a GIL-enabled CPython build, only one thread executes Python bytecode at a time, but a compound source-level operation can still involve multiple interpreter steps, may invoke code that releases the GIL, and is not specified as a general synchronization primitive. If multiple threads modify shared state, use `threading.Lock` or another thread-safe coordination mechanism.

Below is an example illustrating the use of a lock to ensure a thread-safe increment of a shared `counter`:

```python
import threading

counter = 0
counter_lock = threading.Lock()

def safe_increment():
    global counter
    with counter_lock:
        temp = counter
        temp += 1
        counter = temp

# Create and start threads
threads = [threading.Thread(target=safe_increment) for _ in range(1000)]

for t in threads:
    t.start()

for t in threads:
    t.join()

print(f"Counter: {counter}")
```

In this example, `counter_lock` makes the read-modify-write sequence mutually exclusive, so only one thread updates `counter` at a time. Without explicit synchronization, code should not rely on the GIL to make a compound update thread-safe. Locks or other concurrency primitives make that requirement clear and portable.

##### Performance Considerations and Best Practices

- It is important to avoid creating an excessive number of threads, as this can lead to significant context switching overhead and increased memory usage. To manage a fixed number of threads efficiently, thread pools like `concurrent.futures.ThreadPoolExecutor` should be used.
- Minimizing lock contention is good for performance. To achieve this, fine-grained locks can be implemented, and the time spent in critical sections should be minimized to reduce the likelihood of threads waiting for access to shared resources.
- Appropriate synchronization mechanisms, such as locks, semaphores, and condition variables, should be used to coordinate thread activities and prevent data races. This ensures that threads operate safely without corrupting shared data.
- Understanding the Global Interpreter Lock (GIL) is essential, especially in Python. The GIL can limit the effectiveness of threading in CPU-bound applications by allowing only one thread to execute Python bytecode at a time. In such cases, using multiprocessing or other parallelism strategies may be more effective than threading.
- Use daemon threads only for best-effort background work that can be abandoned at shutdown. For important work, prefer non-daemon threads with a clear stop-and-join lifecycle. When a daemon is appropriate, set `daemon=True` when creating the thread rather than relying on the older `setDaemon()` API.


Here are some example code snippets demonstrating various aspects of multithreading in Python:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | [single_worker_thread](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/single_worker_thread.py)  | Introduce the concept of threads by creating a single worker thread.         |
| 2  |  [thread_subclass](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/thread_subclass.py)        | Demonstrate how to create a custom thread class by subclassing `Thread`.     |
| 3  |  [multiple_worker_threads](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/multiple_worker_threads.py)  | Show how to create and manage multiple worker threads.                      |
| 4  |  [race_condition](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/race_condition.py)        | Explain race conditions and their impact on multi-threaded applications.      |
| 5  |  [mutex](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/mutex.py)    | Illustrate the use of mutexes to protect shared resources and avoid race conditions. |
| 6  |  [semaphore](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/semaphore.py)           | Demonstrate the use of semaphores to limit the number of concurrent threads accessing a shared resource. |
| 7  |  [producer_consumer](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/producer_consumer.py)    | Present a classic multi-threading problem (Producer-Consumer) and its solution using synchronization mechanisms like mutexes and condition variables. |
| 8  |  [fetch_parallel](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/fetch_parallel.py)        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources. |
| 9  |  [merge_sort](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/merge_sort.py)           | Demonstrate a threaded merge sort and the limits or potential benefits of parallel work depending on the Python runtime and workload. |
| 10 |  [schedule_every_n_sec](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/schedule_every_n_sec.py)   | Show how to schedule tasks to run periodically at fixed intervals using threads. |
| 11 |  [barrier](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/barrier.py)          | Demonstrate the use of barriers to synchronize multiple threads at a specific point in the execution. |
| 12 |  [thread_local_storage](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/thread_local_storage.py)   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  [thread_pool](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/thread_pool.py)          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  [reader_writer_lock](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/reader_writer_lock.py)   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |
| 15 |  [daemon_demo](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/daemon_demo.py)   | Demonstrate daemon threads, which do not keep the Python process alive and may be stopped during interpreter shutdown. |

#### Examples in JavaScript (Node.js)

Node.js runs JavaScript on an event-loop thread by default, while many asynchronous operations are handled by the runtime and operating system. The `worker_threads` module adds separate JavaScript threads for work that benefits from parallel CPU execution. This is especially useful for CPU-intensive tasks such as image processing or computation that would otherwise block the event loop.

Worker threads are provided by the `worker_threads` module. Each worker has its own V8 isolate and ordinary JavaScript objects are not shared implicitly with the main thread or other workers. Workers usually communicate through message passing, but they can explicitly share memory through objects such as `SharedArrayBuffer`.

##### Creating Worker Threads

To create a new worker thread, you instantiate the `Worker` class from the `worker_threads` module. The worker is initialized with a script (or a code string) to execute:

```javascript
// main.js
const { Worker } = require('worker_threads');

const worker = new Worker('./worker.js'); // Separate file containing worker code

worker.on('message', (message) => {
  console.log(`Received message from worker: ${message}`);
});

worker.on('error', (error) => {
  console.error(`Worker error: ${error}`);
});

worker.on('exit', (code) => {
  console.log(`Worker exited with code ${code}`);
});
```

```javascript
// worker.js
const { parentPort } = require('worker_threads');

parentPort.postMessage('Hello from worker');
```

In this example:

1. **`main.js`** creates a `Worker` instance pointing to the `worker.js` file.  
2. The main thread listens for events:  
   - **`message`**: Triggered when the worker sends data back.  
   - **`error`**: Triggered if an uncaught exception occurs in the worker.  
   - **`exit`**: Triggered when the worker stops execution.  
3. **`worker.js`** obtains a reference to `parentPort` (the communication channel back to the main thread) and sends a message.

##### Handling Communication

Communication between the main thread and workers commonly uses message passing through `postMessage` and `on('message', callback)`. Values are handled through the structured-clone mechanism, with some objects transferable rather than copied. Shared memory is separate and must be created explicitly.

```javascript
// main.js (continued)
worker.postMessage({ command: 'start', data: 'example data' });

// worker.js (continued)
const { parentPort } = require('worker_threads');

parentPort.on('message', (message) => {
  console.log(`Worker received: ${JSON.stringify(message)}`);
  // Perform CPU-intensive task or other operations
  parentPort.postMessage('Processing complete');
});
```

Here, the main thread sends a structured message to the worker with a `command` property and some `data`. The worker, upon receiving it, can process the data and then respond back to the main thread.

##### Worker Termination

Workers can be stopped by the parent or can end from within their own execution.

I. From the main thread, you can call `worker.terminate()`, which returns a Promise resolving to the exit code:

```javascript
// main.js
worker.terminate().then((exitCode) => {
  console.log(`Worker terminated with code ${exitCode}`);
});
```

II. Inside the worker, you can terminate execution using `process.exit()`:

```javascript
// worker.js
process.exit(0); // Immediate exit
```

Terminating a worker stops its execution and eventually releases its resources. Forced termination can interrupt pending work, so cooperative shutdown is preferable when cleanup or data flushing matters.

##### Passing Data to Workers

You can also pass initial data to the worker at creation time through the `Worker` constructor using the `workerData` option:

```javascript
// main.js
const { Worker } = require('worker_threads');

const worker = new Worker('./worker.js', {
  workerData: { initialData: 'Hello' }
});
```

Within **`worker.js`**:

```javascript
// worker.js
const { workerData, parentPort } = require('worker_threads');
console.log(workerData); // { initialData: 'Hello' }

// Do work, then optionally respond
parentPort.postMessage('Worker started with initial data!');
```

This pattern is useful for small or essential bits of configuration data that the worker needs right from startup.

##### Transferring Ownership of Objects

Some objects, such as `ArrayBuffer` and `MessagePort`, can be **transferred** to a worker. Transfer moves ownership instead of copying the underlying resource, and a transferred `ArrayBuffer` becomes detached in the sender.

```javascript
// main.js
const { Worker } = require('worker_threads');
const buffer = new ArrayBuffer(1024);

const worker = new Worker('./worker.js', {
  workerData: buffer,
  transferList: [buffer]
});
```

In this example, ownership of the `ArrayBuffer` moves to the worker, so the main thread can no longer use its contents. This differs from `SharedArrayBuffer`, which is not transferred: both threads retain access to the same shared memory and must coordinate concurrent access, typically with `Atomics`.

##### Using `Atomics` and `SharedArrayBuffer`

When memory is shared through `SharedArrayBuffer`, JavaScript provides the `Atomics` object for atomic operations such as `Atomics.add`, `Atomics.load`, and `Atomics.store`, along with wait/notify operations on supported typed arrays. These primitives provide the synchronization needed when multiple threads access the same shared locations.

Example:

```javascript
// main.js
const { Worker } = require('worker_threads');
const sharedBuffer = new SharedArrayBuffer(4);  // Enough for one 32-bit integer
const sharedArray = new Int32Array(sharedBuffer);

const worker = new Worker('./worker.js', { workerData: sharedBuffer });

worker.on('message', (msg) => {
  console.log('Message from worker:', msg);
  console.log('Final value:', Atomics.load(sharedArray, 0));
});
```

```javascript
// worker.js
const { parentPort, workerData } = require('worker_threads');
const { Atomics, Int32Array } = globalThis;

// Interpret the shared buffer as a 32-bit integer array of length 1
const sharedArray = new Int32Array(workerData);

for (let i = 0; i < 100000; i++) {
  // Atomically increment the integer
  Atomics.add(sharedArray, 0, 1);
}

// Once done, send a message back
parentPort.postMessage('Incrementing done!');
```

In this example:

1. The main thread creates a `SharedArrayBuffer` of 4 bytes (enough space for an `Int32Array` element).  
2. That buffer is passed to the worker.  
3. The worker increments the shared integer atomically 100,000 times using `Atomics.add`.  
4. After the worker reports completion, the main thread reads the final value with `Atomics.load(sharedArray, 0)`.

##### Error Handling

Worker-thread errors need an explicit handling strategy:

```javascript
// main.js
worker.on('error', (error) => {
  console.error('Worker error:', error);
});

// worker.js
try {
  // perform some operation that might throw
  throw new Error('Something went wrong');
} catch (err) {
  // Handle locally or propagate
  parentPort.postMessage({ error: err.message });
  // Optionally re-throw, or process.exit(1) for immediate termination
}
```

If an uncaught exception occurs in the worker, the main thread’s `error` event will fire, allowing you to clean up resources or attempt a restart. Consider carefully whether to handle errors in the worker itself or bubble them up to the main thread.

##### Performance Considerations and Best Practices

- Worker threads are most useful for CPU-intensive JavaScript that would otherwise block the event loop. For ordinary I/O-bound work, Node.js asynchronous APIs are usually simpler and more efficient.
- Large messages can be expensive to copy. Consider transfer lists for ownership transfer, or `SharedArrayBuffer` when shared memory is genuinely needed and the required synchronization is well understood.
- Manage worker lifecycles explicitly. Reuse workers for repeated tasks when appropriate, and shut them down when they are no longer needed.
- Handle worker errors and abnormal exits so failures do not leave the application in an inconsistent state.
- Worker threads are not a security sandbox. Do not use them to execute untrusted code merely because they run in separate V8 isolates.

##### Example: Prime Number Calculation

Below is a complete example of using worker threads to calculate prime numbers, demonstrating data passing, message handling, and worker management.

```javascript
// main.js
const { Worker } = require('worker_threads');

function runService(workerData) {
  return new Promise((resolve, reject) => {
    const worker = new Worker('./primeWorker.js', { workerData });
    worker.on('message', resolve);
    worker.on('error', reject);
    worker.on('exit', (code) => {
      if (code !== 0) {
        reject(new Error(`Worker stopped with exit code ${code}`));
      }
    });
  });
}

runService(10).then((result) => console.log(result)).catch((err) => console.error(err));
```

```javascript
// primeWorker.js
const { parentPort, workerData } = require('worker_threads');

function isPrime(num) {
  for (let i = 2, sqrt = Math.sqrt(num); i <= sqrt; i++) {
    if (num % i === 0) return false;
  }
  return num > 1;
}

const primes = [];
for (let i = 2; i <= workerData; i++) {
  if (isPrime(i)) primes.push(i);
}

parentPort.postMessage(primes);
```

In this example, the main thread delegates the task of finding prime numbers up to a certain limit to a worker thread. The worker calculates the primes and sends the results back to the main thread using `parentPort.postMessage()`.

Here are some example code snippets demonstrating various aspects of multithreading in JavaScript (Node.js):


| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | [single_worker_thread](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/01_single_worker_thread.js)  | Introduce Worker Threads: `isMainThread`, `parentPort`, two-way messaging.         |
| 2  | [thread_subclass](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/02_thread_subclass.js)        | Worker wrapper class; uses `eval()` for function serialization (note: antipattern for production).     |
| 3  | [multiple_worker_threads](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/03_multiple_worker_threads.js)  | Spawn multiple workers dispatched by `workerData.functionName`; track completion.  |
| 4  | [race_condition](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/04_race_condition.js)        | ⚠️ ANTIPATTERN — unsynchronised SharedArrayBuffer counter; result is unpredictable.      |
| 5  | [mutex](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/05_mutex.js)    | Mutex via `Atomics.compareExchange` + `Atomics.wait/notify`; fix for #4. |
| 6  | [semaphore](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/06_semaphore.js)           | Counting semaphore with CAS acquire loop; explains why load+sub is a TOCTOU race. |
| 7  | [producer_consumer](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/07_producer_consumer.js)    | ⚠️ ANTIPATTERN — circular buffer with TOCTOU races on writeIdx/readIdx when using multiple producers/consumers. |
| 8  | [fetch_parallel](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/08_fetch_parallel.js)        | Parallel HTTP fetching with workers; sequential vs. concurrent performance comparison. |
| 9  | [merge_sort](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/09_merge_sort.js)           | Divide-and-conquer parallel merge sort; single-threaded vs. multi-threaded timing. |
| 10 | [schedule_every_n_sec](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/10_schedule_every_n_sec.js)   | Periodic tasks in a worker; two-way message control (`start`/`stop` commands). |
| 11 | [barrier](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/11_barrier.js)          | One-shot barrier: all threads wait at a sync point before any can continue. |
| 12 | [thread_local_storage](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/12_thread_local_storage.js)   | Workers have isolated module scope — global variables are thread-local by default. |
| 13 | [thread_pool](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/13_thread_pool.js)          | Fixed worker pool processing a task queue; worker reuse without re-spawning. |
| 14 | [reader_writer_lock](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/14_reader_writer_lock.js)   | ⚠️ ANTIPATTERN — naive polling reader-writer lock with TOCTOU races on both reader and writer paths. |
| 15 | [message_channel](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/15_message_channel.js)   | `MessageChannel` for direct worker-to-worker communication; `transferList` for ports. |
| 16 | [transferable](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/16_transferable.js)   | Structured clone vs. `ArrayBuffer` transfer (zero-copy, sender detached) vs. `SharedArrayBuffer`. |
| 17 | [atomics_waitasync](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/17_atomics_waitasync.js)   | `Atomics.waitAsync` — non-blocking Promise-based coordination safe on the main thread; contrast with blocking `Atomics.wait`. |
| 18 | [deadlock_antipattern](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/18_deadlock_antipattern.js)   | ⚠️ ANTIPATTERN — two workers each `Atomics.wait` for the other; exits via 3 s timeout. |
| 19 | [worker_error_handling](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/19_worker_error_handling.js)   | `worker.on('error/exit')`; unhandled rejections; `uncaughtException` in worker; cooperative error propagation. |
| 20 | [worker_lifecycle](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/20_worker_lifecycle.js)   | `worker.terminate()` (abrupt); cooperative cancellation via SAB flag; `worker.ref/unref`. |
