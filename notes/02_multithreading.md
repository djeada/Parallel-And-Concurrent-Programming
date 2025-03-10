## Multithreading

**Multithreading** refers to the capability of a CPU, or a single core within a multi-core processor, to execute multiple threads concurrently. A thread is the smallest unit of processing that can be scheduled by an operating system. In a multithreaded environment, a program, or process, can perform multiple tasks at the same time, as each thread runs in the same shared memory space. This can be useful for tasks that are IO-bound, as threads can be used to keep the CPU busy while waiting for IO operations to complete. However, because threads share the same memory, they must be carefully synchronized to avoid issues like race conditions, where two threads attempt to modify the same data concurrently, leading to unpredictable outcomes.

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

* Two ways to create threads in multithreading are using a thread pool or on-demand thread spawning.
* Thread pool pre-spawns threads to reduce the creation costs associated with starting new threads.
* On-demand thread spawning creates threads as needed, which can help in reducing resource wastage.
* However, on-demand thread spawning may slow down the program when threads are needed due to the overhead of creating threads at runtime.

### Worker Threads

* In multithreading, the main thread typically initiates all other threads, which are known as worker threads.
* Worker threads only perform tasks when they are allocated by the main thread or another controlling thread.
* To regulate and limit the number of worker threads, a thread pool can be employed.

A web server process, for example, receives a request and assigns it to a thread from its pool for processing. That thread then follows the main thread's instructions, completes the task, and returns to the pool, allowing the main thread to remain free for other tasks.

### Advantages of Threads over Processes

* Multithreading has several advantages over using multiple processes.
* One key advantage is better responsiveness, allowing a program to remain responsive even when part of it is performing a lengthy operation.
* Another benefit is faster context transitions between threads compared to processes, as threads share the same memory space.
* Threads also improve resource sharing since code, data, and files can be shared across all threads within a process.

### Challenges with Multithreading

- In multithreaded programs, threads share a **common state**, which makes inter-thread communication easier but introduces risks when accessing shared resources.
- A primary concern is maintaining **data consistency**. Without proper synchronization, multiple threads can attempt to read or modify shared data at the same time, causing **race conditions** and unpredictable outcomes.
- **Efficient resource management** is important. Thread creation, context switching, and lock handling introduce **overhead**. If not managed properly, these factors can negate the performance benefits of multithreading.
- Managing **shared memory** is challenging. When multiple threads access the same memory location, inconsistencies can occur unless synchronization mechanisms like locks, mutexes, or semaphores are in place.
- The **nondeterministic** nature of thread scheduling by the operating system complicates debugging and testing. Errors that depend on timing and ordering may only appear sporadically, making them difficult to reproduce and fix.
- Balancing **performance** with thread safety is vital. Techniques such as locking prevent data corruption but may reduce concurrency, increasing wait times and hindering potential speedups.

#### Data Race

- A data race (or **race condition**) happens when the correctness of a multithreaded program depends on the timing or sequence of thread execution, potentially causing errors and unpredictable results.
- Because threads are **preemptively switched** by the OS, programmers have limited control over when a context switch happens, increasing the likelihood of conflicts.
- While preemptive switching removes the burden of manually controlling **task-switching**, it also means a thread can be paused at any point, possibly causing inconsistent or incomplete operations on shared data.

Consider an example: two functions, `funA()` and `funB()`, where `funB()` relies on the output of `funA()`. In a single-threaded program:

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

The execution order becomes unpredictable. If `funB()` runs before `funA()` has completed, the result could be incorrect.

- A data race specifically occurs when two threads **concurrently** access the same memory location, with at least one thread modifying it. This can lead to **memory corruption** if no proper safeguards are in place.
- **Locks** (or other synchronization primitives) are typically used to protect important sections so that only one thread can access specific memory at a time, ensuring data integrity.

**Analogy**:  

*Imagine a busy kitchen with multiple chefs working on the same dish. They share the same utensils and ingredients. Without coordination, two chefs might grab the same tool or ingredient at the same time, causing confusion or mistakes. Likewise, a data race occurs when multiple threads share data without proper synchronization, leading to unpredictable outcomes.*

**Example**:

```cpp
#include <iostream>
#include <thread>
#include <vector>

// Shared counter variable
int counter = 0;

// Function to increment the counter
void incrementCounter(int numIncrements) {
    for (int i = 0; i < numIncrements; ++i) {
        // Read, increment, and write back the counter
        // This is not an atomic operation and can cause race conditions
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

**Possible Output**:

```
Final counter value: 282345
Expected counter value: 1000000
Time taken: 0.023456 seconds

```

**What is happening**:

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

In this scenario, both threads read the same value (100) before either has a chance to write back the incremented value. This leads to lost updates and an incorrect final result.

**What do we mean by a resource?**

In the context of computing and multithreading, a resource refers to any hardware or software component that applications and processes need to operate effectively. This includes elements such as CPU time, memory, storage, network bandwidth, files, and shared data structures. Resources are limited and must be managed efficiently to make sure that multiple threads or processes can access them without conflicts. Proper resource management is necessary for maintaining optimal system performance, preventing bottlenecks, and avoiding issues like deadlocks or excessive contention when multiple threads compete for the same assets.

#### Mutex

- A **mutex** (short for *mutual exclusion*) ensures that only one thread can access a important section of code (and thus shared data) at any given time.
- If one thread holds the mutex, other threads attempting to acquire it will block (or go to sleep) until the mutex is released.

**Analogy**:  

*Imagine a single-stall public restroom. If multiple people try to enter simultaneously, chaos ensues. Instead, a lock on the door ensures only one person can use it at a time. Similarly, a mutex ensures exclusive access to a shared resource.*

**Example**:

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

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

**Possible Output**:

```
Final counter value: 1000000
Expected counter value: 1000000
Time taken: 0.234567 seconds
```

**What is happening**:

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

An **atomic** operation ensures that a read-modify-write sequence completes as one indivisible action. This means no other thread can interrupt or observe a partial update, preventing data races for simple shared variables without needing a heavier synchronization mechanism like a mutex. Atomic operations can apply to various fundamental data types (e.g., `int`, `bool`, `pointer` types) and, in many implementations, to user-defined types that are trivially copyable and do not exceed a certain size (often the size of a machine word).  

In C++, these atomic types are provided by `std::atomic<T>`, and some specialized versions like `std::atomic_flag` offer specific functionalities. The standard guarantees that reads and writes to these types occur as single, uninterruptible steps. Operations like `load`, `store`, `fetch_add`, `fetch_sub`, `compare_exchange`, and similar can all be made atomic.

**What do we gain by using atomics?**

- Atomics utilize **hardware**-level instructions that are lighter than mutexes, enhancing efficiency for simple operations.
- Operations using atomics avoid **contention** since threads don't wait for locks to be released, allowing independent progression.
- Atomics provide **simplicity** for managing basic shared data like counters and flags, reducing the risk of race conditions.

**What do we lose by using atomics?**

- Using atomics requires careful management of **memory** ordering, as incorrect orderings can lead to subtle bugs.
- Atomics are **limited** to simple operations and are not suitable for complex data structures or large objects.
- Ensuring the correctness of higher-level algorithms with atomics can lead to **concurrency** pitfalls such as livelocks or ABA problems.
- In scenarios with heavy contention, atomic operations may not be **faster** than other synchronization methods, depending on hardware and use case.

**Analogy**:

*Imagine a vending machine that instantly dispenses an item the moment you press a button and inserts your bill into a slot—no one can see a partial transaction or grab the bill out mid-transaction. The entire action (paying and getting the item) is handled as a single, uninterruptible event.*

**Example**:

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

**What is happening**:

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

- An operation executed as a single, indivisible step, known as **atomic**, ensures it is free from race conditions.
- The **lock-free** property guarantees that at least one thread will always make progress, even under contention, preventing the system from blocking entirely.
- A **wait-free** guarantee ensures that every thread makes progress within a bounded number of steps, offering maximum fairness and predictability.

#### Deadlock

A **deadlock** occurs when two or more threads are blocked, each waiting for a lock that another thread already holds. Because all threads are waiting on one another, no progress can be made, and the system is effectively stuck.

**Analogy**:  

*Imagine two cars on a narrow one-lane bridge coming from opposite ends. Each driver refuses to back up, and neither can move forward. Both are blocked indefinitely, waiting for the other to yield.*

**Example**:

```cpp
#include <iostream>
#include <thread>
#include <mutex>

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

**What is happening**:

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

A **livelock** occurs when two or more threads actively respond to each other in a way that prevents them from making progress. Unlike a deadlock, the threads are not blocked; they keep "moving," but they continually change their states in a manner that still prevents the system from completing its task.

**Analogy**:  

*Picture two people in a narrow hallway who both step aside to let the other pass—only to keep stepping in the same direction repeatedly. They’re not standing still, but neither can get by the other.*

**Example**:

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

**What is happening**:

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

(Threads keep attempting to acquire both locks,
but they often release them and try again at the
same time, never settling and never fully blocking,
thus making no actual forward progress -> livelock)
```
 
#### Semaphore

A **semaphore** is a synchronization mechanism that uses a counter to control how many threads can access a shared resource at once. Each thread performs an atomic **wait** (or *acquire*) operation before entering the critical section, which decrements the semaphore’s counter. When a thread finishes its work, it performs a **signal** (or *release*) operation, incrementing the counter and allowing other waiting threads to proceed.

**Analogy**:  

*Think of a parking garage with a limited number of spaces. Each car (thread) must check if a space is available before entering (acquire). If no space is free, the car must wait. When a car leaves (release), a space opens up for the next waiting car.*

**Example** (using C++20 counting semaphore):

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

**What is happening**:

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

**Binary Semaphore vs. Mutex**  

There is a common misconception that a binary semaphore and a mutex are equivalent. While both can restrict access to a resource, their primary use cases differ:

- A **mutex** is typically used to gain exclusive ownership over a resource. Only the thread that acquires the mutex can unlock it.
- A **binary semaphore**, although it can only hold one of two possible states (0 or 1), is commonly employed as a **signaling mechanism**. A “producer” thread signals that an event or condition has occurred (e.g., data is ready), and one or more “consumer” threads can then proceed to act on that information.

**Multithreading Automatically Improves Performance**

Many developers believe that incorporating multiple threads always leads to faster execution. However, multithreading can also slow down an application if not designed and tuned properly. The overhead of context switching, synchronization, and resource contention can negate performance gains, especially if the tasks are not well-suited for parallelism.

**More Threads Equals Better Performance**

It is often assumed that creating more threads will consistently boost performance. In reality, once the number of threads exceeds the available CPU cores or the nature of the task’s concurrency limits, performance may degrade. Excessive thread creation can lead to increased scheduling overhead, cache thrashing, and resource contention—ultimately harming efficiency.

**Multithreaded Code Is Always Harder to Write and Maintain**  

While concurrency introduces challenges—such as synchronization, potential race conditions, and timing-related bugs—multithreaded code is not necessarily more difficult to manage than single-threaded code. Modern languages and frameworks provide abstractions (e.g., thread pools, futures, async/await mechanisms) that simplify parallelism. With proper design, testing strategies, and usage of these tools, writing reliable and maintainable multithreaded applications becomes more approachable.

#### Problems for which multithreading is the answer

- Intensive computations, such as large-scale data analysis, scientific simulations, or complex mathematical calculations that require significant processing power.
- External clients sending requests to a process in a random and unpredictable fashion, like a PostgreSQL database handling multiple simultaneous queries from various users.
- Tasks that can be intuitively split into independent processing steps, allowing different threads to handle separate parts of a workflow concurrently.
- Continuous access to a large read-only data set, where multiple threads can efficiently read and process the data without needing to modify it.
- Tasks whose performance would be unacceptable as a single thread, necessitating parallel execution to meet performance requirements.
- Managing concurrent access to multiple resources, such as an operating system coordinating access to hardware components, memory, and peripherals.
- Processing a stream of large data files, enabling different threads to handle different segments of the data simultaneously for faster processing.
- Problems where each step has a clear input and output, facilitating parallel processing of sequential steps in a pipeline.
- Processes where the workload cannot be anticipated, allowing the system to dynamically allocate threads to handle varying loads effectively.
- Real-time data processing tasks, such as financial trading systems that require immediate handling of incoming market data to execute trades without delay.
- Asynchronous I/O operations, where applications perform multiple file reads and writes simultaneously without blocking the main execution thread.
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
- Real-time systems with strict timing requirements need predictable and deterministic execution times, where the unpredictability of thread scheduling can cause issues.
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

### Examples

#### Examples in C++

In C++, every application starts with a single default main thread, represented by the `main()` function. This main thread can create additional threads, which are useful for performing multiple tasks simultaneously. Since C++11, the Standard Library provides the `std::thread` class to create and manage threads. The creation of a new thread involves defining a function that will execute in parallel and passing it to the `std::thread` constructor, along with any arguments required by that function.

##### Creating Threads

A new thread in C++ can be created by instantiating the `std::thread` object. The constructor accepts a callable object (like a function, lambda, or function object) and optional arguments to be passed to the callable object.

```cpp
#include <iostream>
#include <thread>

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

**Advantages**:

- The main program can wait for the thread to **complete**, ensuring synchronization between threads.
- It facilitates proper resource **management** by ensuring threads finish before the program terminates.

**Disadvantages**:
 
- A **drawback** is that the main program may block while waiting, potentially reducing responsiveness.
- It requires careful handling to prevent deadlocks or race **conditions** during synchronization.

```cpp
t1.join(); // Main thread waits for t1 to finish
```

##### Thread Detaching

Using `detach()`, a thread is separated from the `std::thread` object and continues to execute independently. This allows the main thread to proceed without waiting for the detached thread to finish. However, once detached, the thread becomes non-joinable, meaning it cannot be waited on or joined, and it will run independently until completion.

**Advantages**:

- The main program continues without waiting for the detached thread, facilitating fire-and-forget tasks.
- It is useful for **fire-and-forget** tasks.

**Disadvantages**:

- There is no **control** over when the thread finishes.
- There is a risk of resources not being properly managed, as the program might end before the thread completes.

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

Deadlocks occur when two or more threads are waiting for each other to release resources, resulting in a standstill. To avoid deadlocks, it's crucial to lock multiple resources in a consistent order, use try-lock mechanisms, or employ higher-level concurrency primitives like `std::lock` or condition variables.

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

Here, `std::lock` locks both mutexes without risking a deadlock by ensuring that both mutexes are acquired in a consistent order.

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

For cases where synchronization is needed, but mutexes are too heavy-weight, C++ provides atomic operations via the `std::atomic` template. This allows for lock-free programming and can be used to implement simple data structures or counters safely in a multithreaded environment.

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

Each ordering offers different guarantees about how operations on one thread become visible to other threads and in what sequence they appear to happen. Understanding these guarantees can greatly affect both the correctness and performance of concurrent code.

Below is a **comparison table** that summarizes the main C++ memory orderings, their guarantees, common use cases, and potential pitfalls. Use this as a quick reference to decide which ordering is best suited for a particular concurrency scenario.

| **Memory Order**               | **Brief Description**                                                          | **Key Guarantees**                                                                                                                                                                                          | **Common Use Cases**                                                                                                                    | **Pitfalls & Advice**                                                                                                                                        |
|--------------------------------|-------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------|
| **`std::memory_order_relaxed`** | Provides only atomicity, no ordering constraints                             | - The operation itself is atomic (indivisible)  <br/> - No guarantees about visibility or ordering relative to other operations                                                                             | - Simple counters or statistics <br/> - Non-critical flags where ordering doesn’t matter                                                                                     | - Easy to introduce data races if other parts of the program rely on the update’s order <br/> - Great performance but requires careful design                 |
| **`std::memory_order_consume`** | Intended to enforce data dependency ordering (rarely implemented properly)   | - Theoretically only dependent reads are ordered  <br/> - In practice, compilers often treat it like `acquire`                                                                                              | - Very specialized, mostly replaced by `acquire` in real-world code                                                                                                           | - Not well supported by most compilers <br/> - Avoid in portable or production code                                                                                              |
| **`std::memory_order_acquire`** | Prevents following reads/writes from moving before the acquire operation      | - Ensures that subsequent operations see all side effects that happened before a matching `release` <br/> - Acts as a one-way barrier after the load                                                         | - Loading a “ready” flag to know that data is now valid <br/> - Synchronizing consumer who must see the producer’s writes                                                    | - Only ensures that instructions *after* the acquire load can’t be reordered before it <br/> - Must pair with `release` for full producer-consumer semantics |
| **`std::memory_order_release`** | Prevents preceding reads/writes from moving after the release operation       | - Ensures all prior writes are visible to a thread that does an `acquire` on the same atomic <br/> - One-way barrier before the store                                                                        | - Setting a “ready” flag after populating shared data <br/> - Synchronizing producer who writes data before signaling availability                                          | - Doesn’t prevent instructions *after* the release from moving before it <br/> - Must pair with `acquire` to guarantee another thread will observe the updates |
| **`std::memory_order_acq_rel`** | Acquire + Release in one read-modify-write operation                          | - Combines the effects of `acquire` and `release` for RMW ops (e.g., `fetch_add`, `compare_exchange`) <br/> - Ensures no reorder before or after the operation                                              | - Updating shared state in a single atomic step where you must see previous writes and publish new writes (e.g., lock-free structures)                                     | - Can be stronger (thus slower) than needed if you only require a one-way barrier <br/> - Must be used carefully in highly concurrent scenarios              |
| **`std::memory_order_seq_cst`** | Enforces total sequential consistency across all threads                     | - Provides a single, global order of all sequentially consistent operations <br/> - Easiest model to reason about, strongest ordering guarantee                                                             | - When correctness is paramount and performance concerns are secondary <br/> - Prototyping concurrency code before optimizing                                               | - Highest potential performance cost <br/> - May introduce unnecessary fences on weaker architectures                                                        |

**What Do We Gain By Careful Use of Memory Orderings?**

- **Weaker orderings** such as `relaxed`, `acquire`, and `release` can compile to more efficient instructions on some hardware, resulting in better performance compared to using a blanket `seq_cst`.
- Careful use of memory orderings provides **control** by ensuring only the minimal necessary barriers are in place, which helps prevent the use of expensive hardware fences when they are not needed.

**What Do We Lose / Need to Beware Of?**

- Managing memory orderings introduces **complexity**, making it easy to introduce subtle bugs if the chosen ordering is too weak to guarantee the necessary data visibility.
- Code that utilizes specialized memory orderings can suffer from reduced **portability** and become harder to maintain, especially when new developers join the project.
- Using  **overly strong orderings** like `seq_cst` everywhere can lead to over-synchronization, causing potential performance losses by missing out on possible optimizations.

**Analogy**

Imagine you’re coordinating a relay race: 

- A `release` operation is like handing the baton off—ensuring everything you’ve done (run your segment) is finished before the next runner picks it up.  
- An `acquire` operation is the next runner receiving the baton—ensuring they see everything you did (how far you ran, the state of the race) the moment they take it.  
- `relaxed` would be like running without caring about handing the baton off or receiving it properly—fast, but not synchronized.  
- `seq_cst` would be like having a strict official track judge making sure everyone runs in a strictly observed, universal order—less chance of cheating but more overhead.

**Example**

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

**What is happening**:

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

- A **release** operation ensures that all writes before it, including `data.value = 42`, are visible to another thread that performs an acquire operation.
- An **acquire** operation ensures that once `ready` is seen as `true`, the consumer consistently sees the "before-release" state, such as `data.value = 42`.

##### Performance Considerations and Best Practices

- The frequent creation and destruction of threads can be costly, leading to significant overhead. To minimize this, it is advisable to use thread pools or reuse threads, which can reduce the performance impact associated with thread lifecycle management.
- Synchronization mechanisms, such as mutexes, should be used sparingly because excessive synchronization can lead to contention and reduced performance. It is important to apply these mechanisms only when necessary to avoid unnecessary delays and overhead.
- To avoid data races, it is crucial to always protect shared data with appropriate synchronization primitives. This ensures that only one thread can access the data at a time, preventing concurrent modifications that could lead to inconsistent or incorrect data states.
- Utilizing modern features introduced in C++11 and later, such as `std::thread`, `std::mutex`, `std::lock_guard`, and `std::future`, can greatly simplify thread management and help avoid common pitfalls. These features provide robust and standardized ways to handle concurrency, making the code more maintainable and less error-prone.

Here are some example code snippets demonstrating various aspects of multithreading in C++:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/01_single_worker_thread.cpp">single_worker_thread</a>  | Introduce the concept of threads by creating a single worker thread using `std::thread`.         |
| 2  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/02_thread_subclass.cpp">thread_subclass</a>        | Demonstrate how to create a custom thread class by inheriting `std::thread`.     |
| 3  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/03_multiple_worker_threads.cpp">multiple_worker_threads</a>  | Show how to create and manage multiple worker threads using `std::thread`.                      |
| 4  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/04_race_condition.cpp">race_condition</a>        | Explain race conditions and their impact on multi-threaded applications using C++ examples.      |
| 5  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/05_mutex.cpp">mutex</a>    | Illustrate the use of `std::mutex` to protect shared resources and avoid race conditions in C++ applications. |
| 6  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/06_semaphore.cpp">semaphore</a>           | Demonstrate the use of `std::counting_semaphore` to limit the number of concurrent threads accessing a shared resource in C++ applications. |
| 7  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/07_producer_consumer.cpp">producer_consumer</a>    | Present a classic multi-threading problem (Producer-Consumer) and its solution using C++ synchronization mechanisms like `std::mutex` and `std::condition_variable`. |
| 8  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/08_fetch_parallel.cpp">fetch_parallel</a>        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources using C++ threads. |
| 9  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/09_merge_sort.cpp">merge_sort</a>           | Use multi-threading in C++ to parallelize a merge sort algorithm, demonstrating the potential for performance improvements. |
| 10 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/10_schedule_every_n_sec.cpp">schedule_every_n_sec</a>   | Show how to schedule tasks to run periodically at fixed intervals using C++ threads. |
| 11 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/11_barrier.cpp">barrier</a>          | Demonstrate the use of `std::barrier` to synchronize multiple threads at a specific point in the execution. |
| 12 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/12_thread_local_storage.cpp">thread_local_storage</a>   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/13_thread_pool.cpp">thread_pool</a>          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/multithreading/14_reader_writer_lock.cpp">reader_writer_lock</a>   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |

#### Examples in Python

Python provides built-in support for concurrent execution through the `threading` module. While the Global Interpreter Lock (GIL) in CPython limits the execution of multiple native threads to one at a time per process, threading is still useful for I/O-bound tasks, where the program spends a lot of time waiting for external events.

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

Python threads do not have a direct `detach()` method like C++. However, once started, a thread runs independently. The main program can continue executing without waiting for the threads, similar to detached threads in C++. However, you should ensure that all threads complete before the program exits to avoid abrupt termination.

##### Thread Lifecycle and Resource Management

Python threads are automatically managed by the interpreter. However, you should still ensure that threads are properly joined or allowed to finish their tasks to prevent any issues related to resource management or incomplete executions.

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

Deadlocks can occur when multiple threads are waiting for each other to release resources. In Python, you can avoid deadlocks by carefully planning the order of acquiring locks or by using `try-lock` mechanisms.

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

In this example, care must be taken to avoid deadlocks by ensuring that locks are acquired in a consistent order.

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

sem = threading.Semaphore(2)  # Allows up to 2 threads to access the resource

def access_resource(thread_id):
    with sem:
        print(f"Thread {thread_id} is accessing the resource")
        # Simulate some work
        threading.Thread.sleep(1)

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

In multi-threaded Python programs, there is often confusion regarding whether certain operations are truly atomic. This confusion largely stems from the presence of the Global Interpreter Lock (GIL), which ensures that only one thread is executing Python bytecode at any given time. Some developers interpret this to mean that operations like `counter += 1` are automatically safe and cannot cause race conditions. However, this is **not** guaranteed by Python's documentation or design.

While the GIL does prevent multiple threads from running Python *bytecode* simultaneously, many Python operations, including integer increments, actually consist of several steps under the hood (e.g., loading the current value, creating a new integer, and storing it). These intermediate steps can be interleaved with operations from other threads, making race conditions possible if no additional synchronization mechanism is employed. Therefore, if you need to ensure correct and consistent results when multiple threads modify a shared variable, you must use locks (like `threading.Lock`) or other thread-safe data structures.

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

In this example, `counter_lock` ensures that the increment operation is effectively atomic by preventing multiple threads from modifying `counter` at the same time. Without this lock, two or more threads could potentially load the same value of `counter`, increment it independently, and overwrite each other's updates—resulting in an incorrect final value. Keep in mind that **the GIL itself does not guarantee atomicity** for these kinds of operations, which is why locks (or other concurrency primitives) are essential when sharing mutable state across threads.

##### Performance Considerations and Best Practices

- It is important to avoid creating an excessive number of threads, as this can lead to significant context switching overhead and increased memory usage. To manage a fixed number of threads efficiently, thread pools like `concurrent.futures.ThreadPoolExecutor` should be used.
- Minimizing lock contention is crucial for performance. To achieve this, fine-grained locks can be implemented, and the time spent in critical sections should be minimized to reduce the likelihood of threads waiting for access to shared resources.
- Appropriate synchronization mechanisms, such as locks, semaphores, and condition variables, should be used to coordinate thread activities and prevent data races. This ensures that threads operate safely without corrupting shared data.
- Understanding the Global Interpreter Lock (GIL) is essential, especially in Python. The GIL can limit the effectiveness of threading in CPU-bound applications by allowing only one thread to execute Python bytecode at a time. In such cases, using multiprocessing or other parallelism strategies may be more effective than threading.
- For background tasks, daemon threads should be used, as they automatically exit when the program terminates. This can be done by setting `thread.setDaemon(True)`, ensuring that these threads do not prevent the application from closing if they are still running.


Here are some example code snippets demonstrating various aspects of multithreading in Python:

| #  | Example                  | Description                                                                   |
|----|--------------------------|-------------------------------------------------------------------------------|
| 1  | <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/01_single_worker_thread.py">single_worker_thread</a>  | Introduce the concept of threads by creating a single worker thread.         |
| 2  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/02_thread_subclass.py">thread_subclass</a>        | Demonstrate how to create a custom thread class by subclassing `Thread`.     |
| 3  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/03_multiple_worker_threads.py">multiple_worker_threads</a>  | Show how to create and manage multiple worker threads.                      |
| 4  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/04_race_condition.py">race_condition</a>        | Explain race conditions and their impact on multi-threaded applications.      |
| 5  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/05_mutex.py">mutex</a>    | Illustrate the use of mutexes to protect shared resources and avoid race conditions. |
| 6  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/06_semaphore.py">semaphore</a>           | Demonstrate the use of semaphores to limit the number of concurrent threads accessing a shared resource. |
| 7  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/07_producer_consumer.py">producer_consumer</a>    | Present a classic multi-threading problem (Producer-Consumer) and its solution using synchronization mechanisms like mutexes and condition variables. |
| 8  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/08_fetch_parallel.py">fetch_parallel</a>        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources. |
| 9  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/09_merge_sort.py">merge_sort</a>           | Use multi-threading to parallelize a merge sort algorithm, demonstrating the potential for performance improvements. |
| 10 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/10_schedule_every_n_sec.py">schedule_every_n_sec</a>   | Show how to schedule tasks to run periodically at fixed intervals using threads. |
| 11 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/11_barrier.py">barrier          | Demonstrate the use of barriers to synchronize multiple threads at a specific point in the execution. |
| 12 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/12_thread_local_storage.py">thread_local_storage</a>   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/13_thread_pool.py">thread_pool</a>          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/multithreading/14_reader_writer_lock.py">reader_writer_lock</a>   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |

#### Examples in JavaScript (Node.js)

Node.js traditionally uses a single-threaded event loop to handle asynchronous operations. However, since version **10.5.0**, Node.js has included support for worker threads, which allow multi-threaded execution. This is particularly useful for **CPU-intensive** tasks (e.g., image processing, cryptography), which can block the event loop and degrade performance in a purely single-threaded environment.

Worker threads in Node.js are provided by the `worker_threads` module, enabling the creation of additional JavaScript execution contexts. Each worker thread runs in its own isolated V8 instance and does **not** share state with other worker threads or with the main thread. Instead, communication is accomplished by **message passing** and, optionally, by sharing specific memory buffers (e.g., `SharedArrayBuffer`).

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

Communication between the main thread and worker threads is done via **message passing** using `postMessage` and `on('message', callback)`. This serialization-based messaging ensures that no implicit shared state is introduced.

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

Workers can be terminated from **either** the main thread or within the worker itself.

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
process.exit(0); // Graceful exit
```

Terminating the worker ends its event loop and frees its resources. Any pending operations in the worker are discarded once termination begins.

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

Some objects (like `ArrayBuffer` and `MessagePort`) can be **transferred** to a worker, meaning the main thread loses ownership and can no longer use the object once it’s transferred. This can be more efficient than copying large data structures.

```javascript
// main.js
const { Worker } = require('worker_threads');
const buffer = new SharedArrayBuffer(1024);

const worker = new Worker('./worker.js', { workerData: buffer });
```

In this snippet, a `SharedArrayBuffer` is provided to the worker. Both the main thread and the worker thread can access and modify this shared memory concurrently, which is useful for scenarios requiring **high-performance concurrent access** (e.g., streaming or real-time data processing). Synchronization in such cases typically uses `Atomics` (part of JavaScript’s standard library).

##### Using `Atomics` and `SharedArrayBuffer`

When sharing memory (via `SharedArrayBuffer`), JavaScript provides the `Atomics` object for performing atomic operations (e.g., `Atomics.add`, `Atomics.load`, `Atomics.store`). Unlike higher-level synchronization primitives in other languages (like mutexes or semaphores), JavaScript concurrency with `SharedArrayBuffer` and `Atomics` relies on these low-level primitives for correctness.

**Example**:

```javascript
// main.js
const { Worker } = require('worker_threads');
const sharedBuffer = new SharedArrayBuffer(4);  // Enough for one 32-bit integer

const worker = new Worker('./worker.js', { workerData: sharedBuffer });

// Optionally communicate via messages as well
worker.on('message', (msg) => {
  console.log('Message from worker:', msg);
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
4. Both threads can read the final value in `sharedArray[0]` safely, without data races.

##### Error Handling

Proper error handling in multi-threaded environments is crucial:

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

- When handling CPU-intensive tasks, worker threads are particularly advantageous as they can execute computationally heavy operations without blocking the event loop. For tasks that are I/O-bound, however, the Node.js event loop is generally more efficient and sufficient.
- It is important to avoid heavy data transfers between the main thread and worker threads because the process of serialization and deserialization can be inefficient. To enhance efficiency, shared memory structures like `SharedArrayBuffer` should be used when possible, as they allow for direct memory access without the overhead of copying data.
- Proper management of the worker lifecycle is important. Workers should be terminated once they have completed their tasks to prevent resource leaks, which can occur if worker threads remain active unnecessarily and continue consuming system resources.
- Strong error handling is necessary for maintaining stability and reliability in applications that use worker threads. This involves catching and managing exceptions and errors that may occur within worker threads, making sure that these failures do not lead to crashes or unpredictable behavior in the main application.
- Security considerations must be taken into account, as worker threads have access to the complete Node.js API and run in separate V8 instances. To mitigate security risks, it is important to avoid executing untrusted code within worker threads, as this could potentially lead to vulnerabilities and exploits in the system.

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
| 1  | <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/01_single_worker_thread.js">single_worker_thread</a>  | Introduce the concept of threads by creating a single worker thread using Web Workers.         |
| 2  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/02_thread_subclass.js">thread_subclass</a>        | Demonstrate how to create a custom thread class by extending the `Worker` class.     |
| 3  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/03_multiple_worker_threads.js">multiple_worker_threads</a>  | Show how to create and manage multiple worker threads using Web Workers.                      |
| 4  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/04_race_condition.js">race_condition</a>        | Explain race conditions and their impact on multi-threaded applications using JavaScript examples.      |
| 5  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/05_mutex.js">mutex</a>    | Illustrate the use of `Atomics` and `SharedArrayBuffer` to protect shared resources and avoid race conditions in JavaScript applications. |
| 6  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/06_semaphore.js">semaphore</a>           | Demonstrate the use of semaphores to limit the number of concurrent threads accessing a shared resource in JavaScript applications using `Atomics` and `SharedArrayBuffer`. |
| 7  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/07_producer_consumer.js">producer_consumer</a>    | Present a classic multi-threading problem (Producer-Consumer) and its solution using JavaScript synchronization mechanisms like `Atomics` and `SharedArrayBuffer`. |
| 8  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/08_fetch_parallel.js">fetch_parallel</a>        | Showcase a practical application of multi-threading for parallel fetching of data from multiple sources using Web Workers. |
| 9  |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/09_merge_sort.js">merge_sort</a>           | Use multi-threading in JavaScript to parallelize a merge sort algorithm, demonstrating the potential for performance improvements. |
| 10 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/10_schedule_every_n_sec.js">schedule_every_n_sec</a>   | Show how to schedule tasks to run periodically at fixed intervals using JavaScript and Web Workers. |
| 11 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/11_barrier.js">barrier</a>          | Demonstrate the use of barriers to synchronize multiple threads at a specific point in the execution. |
| 12 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/12_thread_local_storage.js">thread_local_storage</a>   | Illustrate the concept of Thread Local Storage (TLS) and how it can be used to store thread-specific data. |
| 13 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/13_thread_pool.js">thread_pool</a>          | Show how to create and use a thread pool to efficiently manage a fixed number of worker threads for executing multiple tasks. |
| 14 |  <a href="https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/multithreading/14_reader_writer_lock.js">reader_writer_lock</a>   | Explain the concept of Reader-Writer Locks and their use for efficient access to shared resources with multiple readers and a single writer. |
