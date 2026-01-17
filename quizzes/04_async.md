# Asynchronous Programming Quiz

Test your understanding of async/await, coroutines, event loops, and futures.

---

#### Q. What type of object does the std::async() function return?

- [x] Future
- [ ] Thread
- [ ] Thread Pool
- [ ] Boolean
- [ ] Promise

#### Q. What is the purpose of a future?

- [ ] It serves as the counterpart to a programming past.
- [ ] It is a task that can be assigned to a thread pool for execution.
- [x] It serves as a placeholder to access a result that may not been computed yet.
- [ ] It allows a program to change how it will function the next time it is run.
- [ ] It provides error handling for asynchronous operations.

#### Q. In Python, which module provides support for high-level asynchronous I/O?

- [x] asyncio
- [ ] threading
- [ ] multiprocessing
- [ ] concurrent
- [ ] sched

#### Q. In Python's asyncio module, which function is used to create a new task?

- [ ] start_task()
- [ ] new_task()
- [x] create_task()
- [ ] schedule_task()
- [ ] add_task()

#### Q. In Python's asyncio module, which method of a future object returns the result or raises the exception of the future?

- [ ] fetch()
- [ ] obtain()
- [x] result()
- [ ] acquire()
- [ ] get_value()

#### Q. In Python's concurrent.futures module, which method is used to cancel a future that has not started execution?

- [ ] stop()
- [ ] terminate()
- [ ] interrupt()
- [x] cancel()
- [ ] halt()

#### Q. In Python's asyncio module, which function runs the event loop until the future (task) completes?

- [ ] run()
- [ ] execute()
- [ ] perform()
- [x] run_until_complete()
- [ ] run_forever()

#### Q. In Python's concurrent.futures module, which method is used to get the result of a future?

- [ ] fetch()
- [ ] obtain()
- [ ] get()
- [x] result()
- [ ] acquire()

#### Q. What is the difference between a coroutine and a regular function?

- [ ] Coroutines are faster
- [ ] Coroutines cannot return values
- [x] Coroutines can pause execution and resume later; regular functions run to completion
- [ ] Coroutines can only be used in web applications
- [ ] There is no difference

#### Q. In Python, what does the 'await' keyword do?

- [ ] Creates a new thread
- [ ] Blocks the entire program
- [x] Pauses the coroutine until the awaited operation completes, allowing other coroutines to run
- [ ] Terminates the coroutine
- [ ] Raises an exception

#### Q. What is an event loop in asynchronous programming?

- [ ] A loop that waits for user events
- [x] A mechanism that runs and coordinates asynchronous tasks by scheduling and executing them
- [ ] A loop that processes network events only
- [ ] A debugging tool for async code
- [ ] A type of infinite loop

#### Q. In Python's asyncio, what is the purpose of asyncio.gather()?

- [ ] To collect garbage from async tasks
- [ ] To merge multiple event loops
- [x] To run multiple coroutines concurrently and collect their results
- [ ] To cancel all running tasks
- [ ] To synchronize async tasks

#### Q. What happens when you call an async function without using 'await'?

- [ ] The function runs synchronously
- [ ] An error is raised immediately
- [x] A coroutine object is returned but the code inside doesn't execute
- [ ] The function runs in a background thread
- [ ] The function runs but results are discarded

#### Q. In Python's asyncio, what is the difference between asyncio.run() and asyncio.create_task()?

- [ ] They do the same thing
- [x] asyncio.run() starts the event loop and runs a coroutine; create_task() schedules a coroutine in an already running loop
- [ ] create_task() is deprecated
- [ ] asyncio.run() is for synchronous code
- [ ] create_task() starts a new thread

#### Q. What is "callback hell" and how does async/await help address it?

- [ ] A situation where callbacks run too fast
- [x] Deeply nested callbacks that make code hard to read; async/await provides a more linear, readable syntax
- [ ] An error that occurs in callback functions
- [ ] A memory leak caused by callbacks
- [ ] Callbacks that never execute

#### Q. In Python's asyncio, what is an asyncio.Queue used for?

- [ ] To prioritize tasks
- [x] To provide async-safe communication between coroutines
- [ ] To store event loop references
- [ ] To queue network requests
- [ ] To manage thread pools

#### Q. What does asyncio.sleep() do differently from time.sleep()?

- [ ] They are identical
- [ ] asyncio.sleep() is faster
- [x] asyncio.sleep() yields control to the event loop; time.sleep() blocks the entire thread
- [ ] asyncio.sleep() only works in threads
- [ ] time.sleep() is asynchronous

#### Q. In Python, what is the purpose of async with (async context manager)?

- [ ] To create async variables
- [x] To properly manage resources in async code that require setup and cleanup
- [ ] To convert sync code to async
- [ ] To handle exceptions in async code
- [ ] To run code in parallel

#### Q. What is a semaphore used for in asyncio?

- [ ] To signal task completion
- [x] To limit the number of concurrent coroutines accessing a resource
- [ ] To prioritize async tasks
- [ ] To handle timeouts
- [ ] To manage memory

#### Q. In Python, what is asyncio.ensure_future() used for?

- [ ] To ensure a future never fails
- [x] To schedule a coroutine or wrap a future, ensuring it will be executed
- [ ] To wait for a future to complete
- [ ] To cancel a future
- [ ] To chain multiple futures

#### Q. What is the difference between asyncio.wait() and asyncio.gather()?

- [ ] They are identical
- [x] wait() gives more control over when to return (first completed, all completed, etc.); gather() waits for all and returns results in order
- [ ] gather() is deprecated
- [ ] wait() is for threads; gather() is for coroutines
- [ ] wait() returns immediately

#### Q. In Python, what is the purpose of async for (async iteration)?

- [ ] To iterate faster
- [x] To iterate over an asynchronous iterator that yields values asynchronously
- [ ] To run a for loop in a thread
- [ ] To iterate over futures
- [ ] To iterate in parallel

#### Q. What happens when an exception is raised inside a coroutine?

- [ ] It is silently ignored
- [ ] It crashes the entire program
- [x] It propagates to the caller when the coroutine is awaited
- [ ] It is logged automatically
- [ ] The coroutine restarts

#### Q. In Python's asyncio, what is a Task?

- [ ] A unit of work assigned to a thread
- [x] A wrapper around a coroutine that schedules it to run on the event loop
- [ ] A synchronous function
- [ ] A type of future that cannot be cancelled
- [ ] A debugging object

#### Q. What is "cooperative multitasking" in the context of asyncio?

- [ ] Multiple event loops working together
- [x] Tasks voluntarily yield control at await points, allowing other tasks to run
- [ ] Threads cooperating to share memory
- [ ] Processes sharing CPU time
- [ ] A type of load balancing

#### Q. In Python, which method is used to cancel an asyncio Task?

- [ ] stop()
- [ ] terminate()
- [x] cancel()
- [ ] abort()
- [ ] kill()

#### Q. What is the difference between asyncio.TimeoutError and concurrent.futures.TimeoutError?

- [ ] They are the same exception
- [x] asyncio.TimeoutError is for async operations; concurrent.futures.TimeoutError is for thread/process pool operations
- [ ] One is deprecated
- [ ] They handle different types of timeouts
- [ ] asyncio.TimeoutError is more detailed

#### Q. In Python's asyncio, what does asyncio.shield() do?

- [ ] Protects code from exceptions
- [x] Protects a coroutine from being cancelled
- [ ] Shields memory from other tasks
- [ ] Prevents timeout errors
- [ ] Hides task exceptions
