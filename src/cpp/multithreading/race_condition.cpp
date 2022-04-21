#include <iostream>
#include <thread>
#include <vector>

int COUNTER = 1;

void foo(int multiplier) {

  if (COUNTER < 10) {
    COUNTER *= multiplier;
    std::cout << "The COUNTER get's multiplied by: " << multiplier << std::endl;
  }
}

auto main() -> int {

  std::vector<std::thread> threads;

  threads.reserve(10);
  for (int i = 0; i < 10; i++) {
    threads.push_back(std::thread(foo, i + 1));
  }

  for (auto &t : threads) {
    t.join();
  }

  std::cout << "The final value of COUNTER is: " << COUNTER << std::endl;

  return 0;
}
