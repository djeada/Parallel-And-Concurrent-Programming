#include <iostream>
#include <chrono>
#include <random>
#include <future>
#include <thread>
#include <vector>
#include <string>

int blocking_function(int i) {
    std::cout << "Running blocking function " << i << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    // Simulate a heavy computation using sleep
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Done running blocking function " << i << std::endl;
    return i;
}

void user_input() {
    std::string user_text;
    while (true) {
        std::cout << "Enter some text: ";
        std::getline(std::cin, user_text);
        std::cout << "User input: " << user_text << std::endl;
    }
}

int main() {
    std::vector<std::future<int>> blocking_tasks;
    for (int i = 0; i < 6; ++i) {
        blocking_tasks.push_back(std::async(std::launch::async, blocking_function, i));
    }

    std::thread input_thread(user_input);

    std::vector<int> results;
    for (auto &task : blocking_tasks) {
        results.push_back(task.get());
    }

    std::cout << "Results: ";
    for (const auto &result : results) {
        std::cout << result << " ";
    }
    std::cout << std::endl;

    input_thread.join();

    return 0;
}
