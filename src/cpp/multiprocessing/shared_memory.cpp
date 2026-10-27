/**
 * C++20 + POSIX: named shared memory with scoped ownership.
 * Create an exclusive name per run and unlink it after opening. The descriptor
 * and mapping have separate owners; child processes inherit the live mapping.
 * A POSIX semaphore protects the shared int; std::mutex is process-local.
 * Compile: g++ -std=c++20 -pthread shared_memory.cpp
 */
#include "../common/posix.hpp"

#include <iostream>
#include <mutex>
#include <string>

int main() try {
    const auto name = "/cpp_shared_" + std::to_string(getpid());
    posix_example::UniqueFd fd(shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, 0600));
    if (fd.get() < 0) posix_example::fail("shm_open");
    posix_example::check(shm_unlink(name.c_str()), "shm_unlink");
    posix_example::check(ftruncate(fd.get(), sizeof(int)), "ftruncate");
    posix_example::SharedObject<int> shared_value(fd.get());
    fd.reset(); // The mapping stays valid after closing the descriptor.
    posix_example::Semaphore semaphore(1, "shared_value");
    posix_example::ChildGroup children(1);
    children.spawn([&] {
        for (int i = 0; i < 5; ++i) {
            std::lock_guard permit(semaphore);
            ++*shared_value.get();
        }
    });
    for (int i = 0; i < 5; ++i) {
        std::lock_guard permit(semaphore);
        --*shared_value.get();
    }
    if (!children.wait_all()) return 1;
    std::cout << "Final value: " << *shared_value.get() << '\n';
    return *shared_value.get() == 0 ? 0 : 1;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
