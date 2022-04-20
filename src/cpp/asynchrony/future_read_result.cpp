#include <iostream>
#include <future>

long long fib(long long n) {
    if (n <= 1)
        return n;
        
    return fib(n - 1) + fib(n - 2);
}

int main()
{
    std::future<long long> future1 = std::async(std::launch::async, fib, 20);
    std::future<long long> future2 = std::async(std::launch::async, fib, 30);

    std::cout << "Main function" << std::endl;
    std::cout << "Fibonacci of 20: " << future1.get() << std::endl;
    std::cout << "Fibonacci of 30: " << future2.get() << std::endl;
    return 0;
}
