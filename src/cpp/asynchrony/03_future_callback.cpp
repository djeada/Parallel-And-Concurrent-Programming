/*
This script demonstrates the use of std::future and callbacks in asynchronous programming. It compares the
asynchronous approach to a synchronous approach for computing square of numbers using slow_square function.

In the asynchronous approach, we create two tasks for computing squares and then create two std::future objects.
We add a callback function 'square_callback' to the std::future objects that will be called when the future is done.

The main function waits for the results of the slow_square function and sets the result for each future, which in turn
triggers the callback functions. This demonstrates how std::future objects can be used for more advanced control flow and
how they can help decouple parts of your code.
*/

#include <iostream>
#include <future>
#include <chrono>
#include <thread>

int slow_square(int x) {
    std::cout << "Starting slow square computation for " << x << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Finished slow square computation for " << x << std::endl;
    return x * x;
}

void square_callback(std::future<int>& future) {
    std::cout << "Square callback called with result: " << future.get() << std::endl;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    std::packaged_task<int(int)> task1(slow_square);
    std::packaged_task<int(int)> task2(slow_square);

    std::future<int> future1 = task1.get_future();
    std::future<int> future2 = task2.get_future();

    std::thread t1(std::move(task1), 3);
    std::thread t2(std::move(task2), 4);

    t1.join();
    t2.join();

    square_callback(future1);
    square_callback(future2);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Asynchronous approach time: " << duration << "ms" << std::endl;

    return 0;
}
