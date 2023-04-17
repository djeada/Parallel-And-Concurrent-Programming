#include <iostream>
#include <thread>
#include <vector>

void foo(int id) {
    std::cout << "Function foo() starts, Thread id: " << id << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Function foo() finishes, Thread id: " << id << std::endl;
}

void bar(int id) {
    std::cout << "Function bar() starts, Thread id: " << id << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Function bar() finishes, Thread id: " << id << std::endl;
}

int main() {
    std::cout << "Main thread starts" << std::endl;

    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(foo, i);
    }

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(bar, i);
    }

    for (auto &thread : threads) {
        thread.join();
    }

    std::cout << "Main thread finishes" << std::endl;

    return 0;
}
