/*
This script demonstrates the use of mutex and condition_variable to protect
access to shared resources in an asynchronous environment. In this example, we
simulate a bank account with two async functions attempting to transfer money
concurrently. The use of a mutex and condition_variable is crucial to prevent
race conditions that could lead to incorrect account balances.
*/

#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class BankAccount {
public:
  explicit BankAccount(int balance) : balance_(balance) {}

  void transfer(int amount) {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this] { return !transfer_in_progress_; });

    transfer_in_progress_ = true;
    std::cout << "Transferring " << amount << "...\n";
    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)); // Simulate some processing time
    balance_ += amount;
    std::cout << "Transfer of " << amount
              << " complete. New balance: " << balance_ << "\n";
    transfer_in_progress_ = false;
    cond_var_.notify_one();
  }

  int balance() const { return balance_; }

private:
  int balance_;
  bool transfer_in_progress_ = false;
  std::mutex mutex_;
  std::condition_variable cond_var_;
};

void transferMoney(BankAccount &account, const std::vector<int> &amounts) {
  for (const auto &amount : amounts) {
    account.transfer(amount);
  }
}

int main() {
  BankAccount account(100);

  auto transfer1 = std::async(std::launch::async, transferMoney,
                              std::ref(account), std::vector<int>{50, -20, 30});
  auto transfer2 =
      std::async(std::launch::async, transferMoney, std::ref(account),
                 std::vector<int>{-10, 60, -10});

  transfer1.wait();
  transfer2.wait();

  std::cout << "Final account balance: " << account.balance() << "\n";

  return 0;
}
