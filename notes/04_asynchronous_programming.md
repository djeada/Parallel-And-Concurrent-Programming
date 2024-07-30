
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

```cpp
#include <iostream>
#include <future>
#include <chrono>

int initial_task() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 5;
}

int next_task(int input) {
    return input * 2;
}

int main() {
    auto future = std::async(std::launch::async, initial_task)
                    .then([](std::future<int> fut) {
                        return next_task(fut.get());
                    });

    std::cout << "Result: " << future.get() << std::endl;
    return 0;
}
```

Here, `initial_task` runs asynchronously, and once it completes, `next_task` is executed with the result.

##### Performance Considerations and Best Practices

- Use asynchronous operations for long-running tasks to keep the main thread responsive.
- Ensure that exceptions are properly handled in asynchronous tasks to prevent crashes.
- Be cautious with resources shared between asynchronous tasks; use synchronization mechanisms like mutexes if necessary.
- Creating too many threads can be costly. Use asynchronous mechanisms and thread pools where appropriate.
- Leverage C++11 and later features like `std::async`, `std::future`, and `std::promise` for cleaner and more efficient asynchronous programming.

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

Here is a table comparing asynchronous programming features in C++, Python, and Node.js:

| Concept    | C++                                                              | Python                                        | Node.js                                                      |
|------------|------------------------------------------------------------------|-----------------------------------------------|--------------------------------------------------------------|
| **Library**    | Standard Library (`std::future`, `std::promise`, `std::async`) <br> Boost.Asio                                         | `asyncio`                                        | Native support (callbacks, Promises)                         |
| **Event Loop** | Depends on the library (e.g., Boost.Asio provides I/O context)   | `asyncio` event loop                            | Event loop provided by the `libuv` library                     |
| **Coroutine**  | C++20 coroutines (`co_await`, `co_yield`)                            | `async def` (coroutine functions)               | Async functions (`async` keyword)                              |
| **Future**     | `std::future` (contains the result of an asynchronous operation)   | `asyncio.Future` (contains the result of a coroutine) | `Promise` (represents the eventual result of an async operation) |
| **Await**      | `co_await` (C++20)                                                 | `await` (used inside `async def`)                 | `await` (used inside async functions)                          |
| **Async Call** | `std::async` or custom implementation with Boost.Asio             | `asyncio.create_task()`, `asyncio.gather()`, etc. | Callbacks, Promises, or `async/await` syntax                   |
| **Task**       | `std::packaged_task` (wraps a callable target)                    | `asyncio.Task` (wraps a coroutine)              | Promises, async functions, or callbacks                      |
