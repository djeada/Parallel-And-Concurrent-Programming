
## Multiprocessing

Multiprocessing is a method of achieving concurrency by creating several processes within a single program. It is important to note that parallelism is not guaranteed by multiprocessing. Processes, unlike threads, do not share any resources by default, hence it makes the most sense to use that approach when you wish to simulatenously complete unrelated tasks.

### Child processes 

The process that spawns new processes is referred to as the parent process. The processes that have been spawned are known as child processes.

The parent processe may:

- wait till it's child is finished; 
- check on the child's condition (it may be running, sleeping, stopped, or zombie); 
- it may want to communicate with the child and request some data back from it; 
- if it's taking too long, it may may just need to kill the child.

#### Zombie Process

A zombie process is one that has finished running but still has an entry in the process table to report to its parent process. A child process always becomes a zombie before being removed from the process table. The parent process reads the exit status of the child process and removes the child process entry from the process table. The zombie isn't taking up much memory or resources; it's just an exit status waiting to be delivered. However, too many zombies might significantly slow down the system.

#### Orphan Process

An orphan process is one whose parent process has finished execution or been terminated without waiting for its child process to finish. An orphan is alive and running, just like any other process; it just has a peculiar name. 

### Communication between processes

To fully take advantage of multiprocessing we need to have a way to communicate between the processes. 

#### Message Passing

Message passing exchanges data via techniques known as inter-process communication (IPC). On the plus side, it is applicable to both local and remote communication (multiprocessing programs can be distributed between remote machines). One disadvantage is a possible "loop-back" configuration, this may result in excessive overhead for big messages. 
   
#### Shared Memory

Shared memory enables programs to access and share data as though they were local to each process's address space. This, however, does not happen automatically, and we must explicitly request the OS to allow two processes to share memory. On the plus side, in a loop-back design, it may be more efficient for massive data. One disadvantage is that it does not generalize well to remote systems and may be more error-prone. 

### Challenges with multiprocessing

The independence of processes from one another is their strength. It's also its weakest point, because processes don't communicate easily with one another.

#### Debugging

Even for single-process applications, debugging may be a time-consuming operation. We often have to run the code line by line to figure out where the bug is hidden. The more processes there are, the more complex the whole operation becomes. Because the debugger can only track one process at a time, you must specify which one it should be. You may not be aware of which one is creating the issues. In that case you may need to repeat the operation  for each process.

#### Deadlocks

A deadlock occurs when two or more processes wait for each other to complete and none of them ever do. Consider the following scenario: during the coronavirus pandemic, many places, including mask retailers, forced you to wear a mask. But what could you do if you didn't have a mask?To enter the shop, you must wear a mask, which you had to buy from the shop in the first place. 

What causes a deadlock to occur?

* Mutual exclusion occurs when a resource is not available for sharing.
* A circular wait is a collection of processes that wait for each other in a circular pattern.
* No preemption is the situation when the operating system may not recover a resource from a process until the process returns it.
* Hold and wait refers to when a process holds at least one resource while waiting for another waiting process to hold another resource. 

### Containers

An alternative to a single multiprocess program is a microservice architecture using containers, each of which executes a single process program. There is a plethora of technologies available that make container orchestration as easy as ABC. The obvious question is how long your child processes should live. If they are only supposed to live for a short time, this approach is excessive. 

### Examples

#### Examples in C++

How to create new processes? 

The C++ standard library only defines concurrent execution synchronization for threads. We must use OS-specific API: 

* In Windows: <code>CreateProcess()</code>
* In Linux: <code>fork()</code>

In every UNIX-based operating system, all processes are created from a single process. The first process is initiated when your system boots up. 

All of the processes that are currently running on your machine are clones of that original process.
When you call <code>fork()</code>', you are just requesting that a clone of your process be created. Everything is copied from the parent process, except for the PID. 

The function <code>waitpid</code> is commonly called in the parent process and causes a process to wait for the execution of a specified process to complete before continuing. 

How do we check if the program in use is the child or the parent?
You may examine the <code>fork()</code>'s return value. It returns different results to the parent and the child. The child receives zero, whereas the parent receives the child's PID. 

When creating a new process, you usually don't want it to do the same thing as the parent.
You spawn the child process to perform a specific task while the parent is busy doing something else.

How do you make a child process run a different program? 

* <code>exec*()</code> family of functions is used to replace an existing process with a new program. 

<code>exec()</code> does not create a new process and run the program in it; instead, it runs the program you provide by replacing it with the existing process. The current process has been terminated. The new one has the same PID as the previous one, but everything else is changed. 

<code>exec()</code> will only return a value if the new program could not be launched for whatever reason. Otherwise, the caller program will simply stop existing, and the new program will run in its place. 

I used the <code>GCC</code> compiler with the following flags to compile the examples:

     g++ file_name.cpp -std=c++17 -pthread -o executable_name

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/spawn_single_process.cpp">spawn single process</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/spawn_multiple_processes.cpp">spawn multiple processes</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/single_pipe.cpp">single pipe</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/deadlock.cpp">deadlock</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/orphan.cpp">orphan</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/cpp/multiprocessing/zombie.cpp">zombie</a>

#### Examples in Python

The <code>os</code> module supports the usage of pure <code>fork()</code>, which works in the same way that we discussed while discussing C++ examples. However, there are superior higher level abstraction alternatives. <code>subprocess</code> and <code>multiprocessing</code> are two modules that may be used to create new processes from a Python script. 

Subprocess creates new processes, but you have no way of communicating with them except from stdin/stdout and any additional APIs the other program may use. Its primary use is to start processes that are fully independent of your own program.

Multiprocessing creates new processes as well, but these processes are meant to run some subtask of your program and easily communicate with one another. The multiprocessing module includes primitives for sharing values between processes.

Furthermore, multiprocessing has an API similar to that of the threading module. Use the <code>Process</code> class to start a new process. The first argument should be the callback function. Then, to begin the process execution, use the <code>start()</code> method. Similarly to threads, if you want the rest of your program to wait for the process to complete, use <code>join()</code>.

Pool is another useful multiprocessing construction. It allows users to run one function in parallel with different sets of inputs.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/spawn_single_process.py">spawn single process</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/process_pool.py">process pool</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/data_sharing_queue.py">data sharing queue</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/single_pipe.py">single pipe</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/python/multiprocessing/deadlock.py">deadlock</a>

#### Examples in JavaScript

In NodeJs, there are four functions for creating a child process: <code>spawn()</code>, <code>fork()</code>, <code>exec()</code>, and <code>execFile()</code>.

Spawn returns a <code>childObject</code>, which may later be used to listen for events. The following events are available: 

     Class: ChildProcess
        Event: 'error'
        Event: 'exit'
        Event: 'close'
        Event: 'disconnect'
        Event: 'message'

There are also a number of potentially useful objects from childObject, which are as follows: 

    Class: ChildProcess
        child.stdin
        child.stdout
        child.stderr
        child.stdio
        child.pid
        child.connected
        child.kill([signal])
        child.send(message[, sendHandle][, callback])
        child.disconnect()

Although multithreading is not supported there is a way to harness the power of a multicore system by using processes. Node.js has a module called cluster designed to support a multiprocessing alternative.

* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/tree/master/src/nodejs/multiprocessing/spawn_single_process">spawn single process</a>
* <a href="https://github.com/djeada/Parallel-and-Concurrent-Programming/blob/master/src/nodejs/multiprocessing/spawn_multiples_processes/main.js">spawn multiple processes</a>
