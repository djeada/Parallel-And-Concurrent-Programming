#include <chrono>
#include <future>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

// Function to simulate a heavy computation
int blocking_function(int i) {
  try {
    std::cout << "Running blocking function " << i << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    // Simulate a heavy computation using sleep
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Done running blocking function " << i << std::endl;
    return i;
  } catch (const std::exception &e) {
    std::cerr << "Exception in blocking function " << i << ": " << e.what() << '\n';
    return -1;
  }
}

// Function to handle user input
void user_input(std::atomic<bool> &running) {
  std::string user_text;
  while (running) {
    std::cout << "Enter some text: ";
    if (std::getline(std::cin, user_text)) {
      std::cout << "User input: " << user_text << std::endl;
    } else {
      running = false;
    }
  }
}

int main() {
  std::vector<std::future<int>> blocking_tasks;
  for (int i = 0; i < 6; ++i) {
    blocking_tasks.push_back(
        std::async(std::launch::async, blocking_function, i));
  }

  std::atomic<bool> running(true);
  std::thread input_thread(user_input, std::ref(running));

  std::vector<int> results;
  for (auto &task : blocking_tasks) {
    try {
      results.push_back(task.get());
    } catch (const std::exception &e) {
      std::cerr << "Exception in main: " << e.what() << '\n';
      results.push_back(-1);
    }
  }

  std::cout << "Results: ";
  for (const auto &result : results) {
    std::cout << result << " ";
  }
  std::cout << std::endl;

  running = false;
  input_thread.join();

  return 0;
}
