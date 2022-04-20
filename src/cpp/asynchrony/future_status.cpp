#include <iostream>
#include <future>
#include <thread>

unsigned int  sleep_for_n_seconds(unsigned int n) {
  std::this_thread::sleep_for(std::chrono::seconds(n));
  return n;
}

int main()
{
    int n = 10;
  
    std::future<unsigned int > future = std::async(std::launch::async, sleep_for_n_seconds, n);

    while (1)
    {
        auto status = future.wait_for(std::chrono::seconds(1));

        switch (status)
        {
            case std::future_status::ready:
                std::cout << "future status: ready" << std::endl;
                std::cout << future.get() << std::endl;
                return 0;
            case std::future_status::deferred:
                std::cout << "future status: deferred" << std::endl;
                break;
            case std::future_status::timeout:
                std::cout << "future status: timeout" << std::endl;
                break;
            default:
                break;
        }

    } 

    return 0;
    }
