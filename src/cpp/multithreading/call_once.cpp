/**
 * Thread-Safe One-Time Initialization with std::call_once
 *
 * std::call_once guarantees a callable executes exactly once even when
 * multiple threads race to invoke it. Other threads block until the
 * initializer completes.
 *
 * Key concepts:
 * - std::once_flag tracks whether initialization has run
 * - std::call_once is thread-safe: exactly one invocation runs to completion
 * - Other concurrent callers block until the invocation finishes
 * - The canonical pattern for lazy, thread-safe initialization / singletons
 *
 * Compile: g++ -std=c++11 -pthread call_once.cpp
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// ─── Example 1: lazy one-time initialization ─────────────────────────────────

std::once_flag init_flag;

void expensive_init() {
    std::cout << "  [Init] Starting expensive setup...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    std::cout << "  [Init] Setup complete.\n";
}

void worker(int id) {
    // All threads call this, but expensive_init runs only once
    std::call_once(init_flag, expensive_init);
    std::cout << "Worker " << id << " proceeding after init.\n";
}

void demo_call_once() {
    std::cout << "=== call_once: 6 threads race to initialize ===\n";
    std::vector<std::thread> threads;
    for (int i = 0; i < 6; ++i) {
        threads.emplace_back(worker, i);
    }
    for (auto& t : threads) t.join();
}

// ─── Example 2: thread-safe singleton ────────────────────────────────────────

class DatabaseConnection {
public:
    static DatabaseConnection& instance() {
        std::call_once(init_flag_, &DatabaseConnection::create);
        return *instance_;
    }

    void query(const std::string& sql) const {
        std::cout << "  [DB] Executing: " << sql << "\n";
    }

private:
    DatabaseConnection() {
        std::cout << "  [DB] Connection opened (runs once).\n";
    }

    static void create() {
        instance_.reset(new DatabaseConnection());
    }

    static std::once_flag init_flag_;
    static std::unique_ptr<DatabaseConnection> instance_;
};

std::once_flag DatabaseConnection::init_flag_;
std::unique_ptr<DatabaseConnection> DatabaseConnection::instance_;

void db_user(int id) {
    DatabaseConnection::instance().query("SELECT * FROM table WHERE id=" +
                                         std::to_string(id));
}

void demo_singleton() {
    std::cout << "\n=== Singleton: 4 threads share one connection ===\n";
    std::vector<std::thread> threads;
    for (int i = 1; i <= 4; ++i) {
        threads.emplace_back(db_user, i);
    }
    for (auto& t : threads) t.join();
}

int main() {
    demo_call_once();
    demo_singleton();
    return 0;
}
