/**
 * C++20 + POSIX: scoped file locks and owned worker processes.
 * Use a private temporary output directory so concurrent runs are independent.
 * Record writes handle partial transfers and EINTR while holding an advisory
 * fcntl lock. Every participating process must follow the locking protocol.
 * Compile: g++ -std=c++20 -pthread process_manager.cpp
 */
#include "../common/posix.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <span>
#include <string>

class FileLock {
public:
    explicit FileLock(int fd) : fd_(fd) {
        struct flock lock{};
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        while (fcntl(fd_, F_SETLKW, &lock) < 0) if (errno != EINTR) posix_example::fail("fcntl lock");
    }
    ~FileLock() {
        struct flock lock{};
        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        fcntl(fd_, F_SETLK, &lock);
    }
    FileLock(const FileLock&) = delete;
    FileLock& operator=(const FileLock&) = delete;

private:
    int fd_;
};

int main() try {
    posix_example::TemporaryDirectory directory;
    const auto output = directory.path() / "results.txt";
    posix_example::open_file(output.c_str(), O_WRONLY | O_CREAT | O_EXCL);
    posix_example::ChildGroup children(5);
    for (int id = 0; id < 5; ++id) {
        children.spawn([&, id] {
            auto fd = posix_example::open_file(output.c_str(), O_WRONLY | O_APPEND);
            FileLock lock(fd.get());
            const auto line = "Worker " + std::to_string(id) + " (PID: "
                              + std::to_string(getpid()) + ") completed\n";
            posix_example::write_all(fd.get(), std::as_bytes(std::span(line)));
        });
    }
    if (!children.wait_all()) return 1;
    std::ifstream results(output);
    if (!results) throw std::runtime_error("Cannot read worker results");
    std::cout << "Results from workers:\n" << results.rdbuf();
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
