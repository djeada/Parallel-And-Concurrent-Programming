/**
 * Mutex Protection in Async Tasks
 *
 * This example demonstrates protecting shared resources
 * when accessed by multiple async tasks.
 *
 * Key concepts:
 * - Multiple async tasks may access shared data concurrently
 * - Mutex ensures only one task modifies data at a time
 * - condition_variable coordinates access order
 * - Essential for maintaining data consistency
 *
 * Without proper synchronization, concurrent modifications
 * would cause data races and incorrect results.
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
    explicit BankAccount(int initial_balance)
        : balance_(initial_balance) {}

    void transfer(int amount) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !transfer_in_progress_; });

        transfer_in_progress_ = true;
        std::cout << "Transferring " << amount << "...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        balance_ += amount;
        std::cout << "New balance: " << balance_ << "\n";
        transfer_in_progress_ = false;
        cv_.notify_one();
    }

    int balance() const { return balance_; }

private:
    int balance_;
    bool transfer_in_progress_ = false;
    std::mutex mutex_;
    std::condition_variable cv_;
};

void do_transfers(BankAccount& account, const std::vector<int>& amounts) {
    for (int amount : amounts) {
        account.transfer(amount);
    }
}

int main() {
    BankAccount account(100);

    auto task1 = std::async(std::launch::async, do_transfers,
                            std::ref(account), std::vector<int>{50, -20, 30});
    auto task2 = std::async(std::launch::async, do_transfers,
                            std::ref(account), std::vector<int>{-10, 60, -10});

    task1.wait();
    task2.wait();

    std::cout << "Final balance: " << account.balance() << "\n";
    std::cout << "Expected: " << (100 + 50 - 20 + 30 - 10 + 60 - 10) << "\n";

    return 0;
}
