
## Asynchrony

Asynchronous programming is a technique used to achieve concurrency, where tasks can be executed independently without waiting for other tasks to finish. It allows for nonblocking behavior, in contrast to synchronous execution that waits for one task to complete before starting the next task.

Asynchronous programming is particularly useful for tasks that involve I/O operations, such as fetching data from a database, where waiting for the data retrieval could freeze the user interface.

## Building Blocks of Asynchronous Programming

Asynchronous programming offers non-blocking execution, which is especially beneficial for I/O-bound operations. The two main pillars of this paradigm are the event loop and async functions.

### Event Loop

The event loop is the engine driving asynchronous programming. It constantly checks for and executes tasks.

**Key Responsibilities**:
1. **Task Management**: Registers and schedules tasks for execution.
2. **Event Dispatch**: Handles I/O operations, routing them to the correct handlers upon completion.
3. **Timer Management**: Manages timeouts and scheduled tasks.
4. **Concurrency**: Enables concurrent task execution in even single-threaded environments.

### Async Functions and Traditional Coroutines

- **Traditional Coroutines**: General mechanisms that allow functions to pause and resume. In Python, they're often implemented using generators (`yield`).

- **Async Functions**: A specialized form of coroutines tailored for asynchronous operations. Defined using `async def`, these functions can pause their execution with `await` to wait for other async operations.

**Async Function Example**:

```python
async def fetch_data():
    # Simulate data fetching with a sleep
    await asyncio.sleep(3)
    return "Data fetched"
```

### Futures and Asyncio Tasks

- **Futures**: Represent the eventual results of asynchronous operations. They signal the operation's state, such as pending or completed.

- **Tasks**: A kind of Future that wraps around async functions. The event loop uses Tasks to manage the execution of async functions.

Working with Futures:

```python
import asyncio

async def main():
    loop = asyncio.get_event_loop()
    future = loop.create_future()

    async def set_result():
        await asyncio.sleep(2)
        future.set_result("Hello, world!")

    loop.create_task(set_result())
    result = await future
    print(result)

asyncio.run(main())
```

## Asynchrony vs Multithreading

Asynchrony and multithreading are distinct concepts but can be used together to achieve parallelism. The key difference is that asynchronous functions switch cooperatively, while threads switch preemptively.

1. Synchronous execution with a single thread:

```
single thread: AAAABBBBBBBBBBBBBBBBBBBBBBCCCCCCCCCC
```

2. Synchronous execution with multiple threads:

```
thread 1: AAAAAAAAAAAA---------------------------------

thread 2: ------------BBBBBB--------------------------

thread 3: ------------------CCCCCCCCCCCCCCCCCCCCCCCCCCC
```

3. Asynchronous execution with a single thread:

```
single thread: AAAABBBBAAAACCCAAABBBCCCCCBBBBBBBBBCCCCC
```

4. Asynchronous execution with multiple threads:

```
thread 1: AAAAAAAAA-----
thread 2: ---BBBBBBBBBBB
thread 3: ----CCCC------
```

## Challenges and Considerations in Asynchronous Programming

Asynchronous functions, distinct from multithreading, utilize a cooperative approach. Here, tasks willingly give up control, rather than being preemptively interrupted. This cooperative model offers advantages:

- **Elimination of Locks**: No need for complex synchronization methods, reducing overhead.
- **Efficient Task Switching**: Transitions between tasks are swift, without the heavyweight context switches seen in threads.

But, it's not without challenges:

1. **Working with Synchronous Functions**:
   - **Challenge**: Embedding synchronous code in an asynchronous context can lead to blocking, undermining the efficiency of your async system.
   - **Solution**: Utilize `run_in_executor` to offload synchronous operations to separate threads. Alternatively, rewrite synchronous code to be asynchronous.

2. **Error Handling**:
   - **Challenge**: Async code's error dynamics differ from synchronous code. Unhandled exceptions in coroutines could disrupt the event loop.
   - **Solution**: Always implement robust exception handling within coroutines. Understand how errors can ripple through your async operations.

3. **Debugging Asynchronous Code**:
   - **Challenge**: The non-linear flow of asynchronous applications can make traditional debugging tricky.
   - **Solution**: Leverage detailed logging, and make use of specialized tools like `asyncio.debug()`. They provide insights into task transitions and pauses.

4. **Scalability Considerations**:
   - **Challenge**: While async shines for I/O-bound operations, CPU-bound tasks might not benefit similarly.
   - **Solution**: For CPU-intensive workloads, consider a hybrid approach. Combine multithreading and multiprocessing to extract maximum performance.

5. **Handling Backpressure**:
   - **Challenge**: In systems where data production and consumption rates differ, you risk overloading parts of the system.
   - **Solution**: Implement mechanisms like buffering, throttling, or load shedding to regulate data flow, ensuring no component gets overwhelmed.

6. **Testing Asynchronous Code**:
   - **Challenge**: The inherent unpredictability in asynchronous task execution complicates testing.
   - **Solution**: Adopt testing tools, such as `pytest-asyncio`, built explicitly for asynchronous paradigms. These tools can simulate various scenarios, ensuring your async code behaves as expected.

## Examples

### Examples in C++

Asynchronous programming in C++ is conceptually similar to Python but has its own set of libraries and constructs. C++ offers several options for asynchronous programming, including the C++ Standard Library, Boost.Asio, and third-party libraries.

### C++ Standard Library

The C++11 standard introduced support for multithreading and concurrency, including features for asynchronous programming. The most commonly used constructs for asynchronous programming in C++ are `std::async`, `std::future`, and `std::promise`.

#### std::async

`std::async` is a function template that allows you to run a function asynchronously, returning a `std::future` that represents the result of the computation. It can execute tasks concurrently or lazily, depending on the launch policy.

Example of using `std::async`:

```cpp
#include <iostream>
#include <future>

int add(int a, int b) {
    return a + b;
}

int main() {
    std::future<int> result = std::async(std::launch::async, add, 10, 20);
    std::cout << "Result: " << result.get() << std::endl;
    return 0;
}
```

#### std::future and std::promise
 
std::future is a class template that represents the result of a computation that may not have completed yet. A `std::promise` is a class template that is used to set the value (or exception) of a `std::future`. It provides a way to communicate the result of a computation from one thread to another.

Example of using `std::future` and `std::promise`:

```cpp
#include <iostream>
#include <thread>
#include <future>

void add(std::promise<int> &&promise, int a, int b) {
    promise.set_value(a + b);
}

int main() {
    std::promise<int> promise;
    std::future<int> result = promise.get_future();

    std::thread t(add, std::move(promise), 10, 20);
    std::cout << "Result: " << result.get() << std::endl;
    t.join();

    return 0;
}
```

I used the <code>GCC</code> compiler with the following flags to compile the examples:

```
g++ file_name.cpp -std=c++17 -pthread -o executable_name
```

| No. | Filename                                                                                         | Description                                              |
|-----|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| 1   | [01_basic_async.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/01_basic_async.cpp) | Create and start a basic asynchronous task              |
| 2   | [02_future_create_task.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/02_future_create_task.cpp) | Create a task using Future and run it asynchronously     |
| 3   | [03_future_read_result.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/03_future_read_result.cpp) | Read the result of a completed Future task               |
| 4   | [04_pause_resume.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/04_pause_resume.cpp) | Pause and resume asynchronous tasks                      |
| 5   | [05_run_heavy_functions.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/05_run_heavy_functions.cpp) | Execute heavy functions asynchronously                   |
| 6   | [06_data_sharing_queue.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/06_data_sharing_queue.cpp) | Share data between asynchronous tasks using a Queue      |
| 7   | [07_semaphore.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/07_semaphore.cpp) | Control access to shared resources with a Semaphore      |
| 8   | [08_producer_consumer.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/08_producer_consumer.cpp) | Implement a producer-consumer pattern asynchronously     |
| 9   | [09_fetch_parallel.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/09_fetch_parallel.cpp) | Fetch data in parallel using async tasks                 |
| 10  | [10_mutex.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/10_mutex.cpp) | Use a Mutex to synchronize access to shared resources    |
| 11  | [11_barrier.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/11_barrier.cpp) | Synchronize multiple asynchronous tasks using a Barrier |
| 12  | [12_async_generator.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/12_async_generator.cpp) | Create and use asynchronous generators                   |
| 13  | [13_async_server.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/13_async_server.cpp) | Implement an asynchronous server                         |
| 14  | [14_distributed_computing.cpp](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/cpp/asynchrony/14_distributed_computing.cpp) | Demonstrate distributed computing with async tasks       |

### Examples in Python

Asynchronous programming in Python is made possible through the `asyncio` library. In order to understand and utilize asynchronous programming, it's essential to grasp the following concepts:

1. async: The `async` keyword is used to define a coroutine function. When placed before `def`, it signifies that the function is asynchronous and can be paused and resumed during execution. This allows other tasks to run concurrently, improving the efficiency of I/O-bound operations.

An async function cannot call a non-async function directly, as it would block the event loop. However, you can use `loop.run_in_executor()` to run synchronous functions in a separate thread, or refactor the non-async function to be async.

Example:

```python
async def async_function():
await asyncio.sleep(1)
return "Async result"
```

2. await: The await keyword is used within a coroutine to call another coroutine and pause the current one until the called coroutine finishes. This allows other tasks to run while the current coroutine is waiting for the operation to complete. The await keyword can only be used inside coroutines.

Example:

```python
async def main():
    result = await async_function()
    print(result)
```

3. Task switching: In asynchronous programming, tasks switch cooperatively, meaning that they voluntarily yield control to other tasks by using await. The event loop schedules and manages the execution of these tasks, ensuring that they are run concurrently without blocking each other.

When a coroutine encounters an `await` expression, it yields control back to the event loop, which can then execute other tasks. Once the awaited operation is complete, the event loop resumes the coroutine from where it left off.

4. Running the event loop: The event loop is responsible for executing and managing asynchronous tasks. You can use `asyncio.run()` to run a coroutine as the main entry point of your asynchronous program. This function takes care of creating and running the event loop, executing the coroutine, and closing the loop when it's done.

Example:

```python
asyncio.run(main())
```

5. Mixing async and non-async functions: As mentioned earlier, you cannot directly call a non-async function from within an async function. However, you can use loop.run_in_executor() to run synchronous functions in a separate thread, allowing the event loop to continue running other tasks concurrently.

Example:

```python
import asyncio
from concurrent.futures import ThreadPoolExecutor

def sync_function():
    time.sleep(1)
    return "Sync result"

async def main():
    loop = asyncio.get_event_loop()
    with ThreadPoolExecutor() as executor:
        result = await loop.run_in_executor(executor, sync_function)
        print(result)

asyncio.run(main())
```
| No. | Filename                                                                                         | Description                                              |
|-----|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| 1   | [01_basic_async.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/01_basic_async.py) | Create and start a basic asynchronous task              |
| 2   | [02_future_create_task.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/02_future_create_task.py) | Create a task using Future and run it asynchronously     |
| 3   | [03_future_read_result.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/03_future_read_result.py) | Read the result of a completed Future task               |
| 4   | [04_pause_resume.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/04_pause_resume.py) | Pause and resume asynchronous tasks                      |
| 5   | [05_run_heavy_functions.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/05_run_heavy_functions.py) | Execute heavy functions asynchronously                   |
| 6   | [06_data_sharing_queue.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/06_data_sharing_queue.py) | Share data between asynchronous tasks using a Queue      |
| 7   | [07_semaphore.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/07_semaphore.py) | Control access to shared resources with a Semaphore      |
| 8   | [08_producer_consumer.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/08_producer_consumer.py) | Implement a producer-consumer pattern asynchronously     |
| 9   | [09_fetch_parallel.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/09_fetch_parallel.py) | Fetch data in parallel using async tasks                 |
| 10  | [10_mutex.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/10_mutex.py) | Use a Mutex to synchronize access to shared resources    |
| 11  | [11_barrier.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/11_barrier.py) | Synchronize multiple asynchronous tasks using a Barrier |
| 12  | [12_async_generator.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/12_async_generator.py) | Create and use asynchronous generators                   |
| 13  | [13_async_server.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/13_async_server.py) | Implement an asynchronous server                         |
| 14  | [14_distributed_computing.py](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/python/asynchrony/14_distributed_computing.py) | Demonstrate distributed computing with async tasks       |

### Examples in JavaScript


Asynchronous programming in Node.js is a fundamental concept, as it is designed to handle non-blocking I/O operations efficiently. The key components of asynchronous programming in Node.js include:

1. Callbacks: Functions that are passed as arguments to other functions and are called when an asynchronous operation is completed.
2. Promises: Objects that represent the eventual completion (or failure) of an asynchronous operation and its resulting value.
3. Async/await: Modern syntax that makes writing asynchronous code easier and more readable, built on top of Promises.

#### Callbacks

Callbacks are a widely used pattern for handling asynchronous operations in Node.js. They are functions that are passed as arguments to other functions and are called when an operation is completed.

Example:

```javascript
const fs = require('fs');

fs.readFile('example.txt', 'utf8', (err, data) => {
  if (err) {
    console.error(err);
  } else {
    console.log(data);
  }
});
```

#### Promises

Promises are a more modern approach for handling asynchronous operations in Node.js. A Promise represents the eventual result of an asynchronous operation and has three states:

* Pending: Initial state, neither fulfilled nor rejected.
* Fulfilled: The operation completed successfully, and the promise has a resulting value.
* Rejected: The operation failed, and the promise has a reason for the failure.

Example:

javascript

const fs = require('fs').promises;

fs.readFile('example
.txt', 'utf8')
  .then(data => {
    console.log(data);
  })
  .catch(err => {
    console.error(err);
  });

#### Async/await

Async/await is a more modern and readable syntax for writing asynchronous code in Node.js, built on top of Promises. It allows you to write asynchronous code that looks similar to synchronous code, making it easier to understand and maintain.

Example:

```javascript
const fs = require('fs').promises;

async function main() {
  try {
    const data = await fs.readFile('example.txt', 'utf8');
    console.log(data);
  } catch (err) {
    console.error(err);
  }
}

main();
```

| No. | Filename                                                                                         | Description                                              |
|-----|--------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| 1   | [01_basic_async.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/01_basic_async.js) | Create and start a basic asynchronous task              |
| 2   | [02_future_create_task.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/02_future_create_task.js) | Create a task using Future and run it asynchronously     |
| 3   | [03_future_read_result.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/03_future_read_result.js) | Read the result of a completed Future task               |
| 4   | [04_pause_resume.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/04_pause_resume.js) | Pause and resume asynchronous tasks                      |
| 5   | [05_run_heavy_functions.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/05_run_heavy_functions.js) | Execute heavy functions asynchronously                   |
| 6   | [06_data_sharing_queue.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/06_data_sharing_queue.js) | Share data between asynchronous tasks using a Queue      |
| 7   | [07_semaphore.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/07_semaphore.js) | Control access to shared resources with a Semaphore      |
| 8   | [08_producer_consumer.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/08_producer_consumer.js) | Implement a producer-consumer pattern asynchronously     |
| 9   | [09_fetch_parallel.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/09_fetch_parallel.js) | Fetch data in parallel using async tasks                 |
| 10  | [10_mutex.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/10_mutex.js) | Use a Mutex to synchronize access to shared resources    |
| 11  | [11_barrier.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/11_barrier.js) | Synchronize multiple asynchronous tasks using a Barrier |
| 12  | [12_async_generator.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/12_async_generator.js) | Create and use asynchronous generators                   |
| 13  | [13_async_server.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/13_async_server.js) | Implement an asynchronous server                         |
| 14  | [14_distributed_computing.js](https://github.com/djeada/Parallel-And-Concurrent-Programming/blob/master/src/js/asynchrony/14_distributed_computing.js) | Demonstrate distributed computing with async tasks       |

### Summary

| Concept    | C++                                                              | Python                                        | Node.js                                                      |
|------------|------------------------------------------------------------------|-----------------------------------------------|--------------------------------------------------------------|
| Library    | Standard Library (std::future, std::promise, std::async) <br> Boost.Asio                                         | asyncio                                        | Native support (callbacks, Promises)                         |
| Event Loop | Depends on the library (e.g., Boost.Asio provides I/O context)   | asyncio event loop                            | Event loop provided by the libuv library                     |
| Coroutine  | C++20 coroutines (co_await, co_yield)                            | async def (coroutine functions)               | Async functions (async keyword)                              |
| Future     | std::future (contains the result of an asynchronous operation)   | asyncio.Future (contains the result of a coroutine) | Promise (represents the eventual result of an async operation) |
| Await      | co_await (C++20)                                                 | await (used inside async def)                 | await (used inside async functions)                          |
| Async Call | std::async or custom implementation with Boost.Asio             | asyncio.create_task(), asyncio.gather(), etc. | Callbacks, Promises, or async/await syntax                   |
| Task       | std::packaged_task (wraps a callable target)                    | asyncio.Task (wraps a coroutine)              | Promises, async functions, or callbacks                      |

