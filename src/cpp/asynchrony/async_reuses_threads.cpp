#include <iostream>
#include <future>
#include <thread>

void foo() {
    std::cout << "Worker thread id: " << std::this_thread::get_id() << std::endl;
}

int main()
{
      std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;

    std::future<void> future1 = async(std::launch::async, foo);

    f1.get(); 

    std::future<void> future2 =async(std::launch::async, foo);

    f2.get();

    std::future<void> future3 = async(std::launch::async, foo);

    f3.get();

    return 0;
}
