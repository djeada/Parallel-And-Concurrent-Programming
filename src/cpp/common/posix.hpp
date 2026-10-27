#pragma once

// Shared header; run ../multiprocessing/basic_process.cpp, not this file.
// Build/run examples from this directory are listed in README.md.
//
// Small C++20 owners for the POSIX examples. Fork-based tasks below are for a
// single-threaded parent; use posix_spawn to launch programs from threaded code.

#include <array>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <semaphore.h>
#include <span>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>
#include <utility>
#include <vector>

namespace posix_example {

[[noreturn]] inline void fail(const char* operation) {
    throw std::system_error(errno, std::generic_category(), operation);
}

inline void check(int result, const char* operation) {
    if (result < 0) fail(operation);
}

class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) noexcept : fd_(fd) {}
    ~UniqueFd() { reset(); }
    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;
    UniqueFd(UniqueFd&& other) noexcept : fd_(other.release()) {}
    UniqueFd& operator=(UniqueFd&& other) noexcept {
        if (this != &other) reset(other.release());
        return *this;
    }
    int get() const noexcept { return fd_; }
    int release() noexcept { return std::exchange(fd_, -1); }
    void reset(int replacement = -1) noexcept {
        if (fd_ >= 0) ::close(fd_); // Do not retry close(): the fd may be reused.
        fd_ = replacement;
    }

private:
    int fd_;
};

inline UniqueFd open_file(const char* path, int flags, mode_t mode = 0600) {
    const int fd = ::open(path, flags | O_CLOEXEC, mode);
    if (fd < 0) fail("open");
    return UniqueFd(fd);
}

struct Pipe {
    UniqueFd reader;
    UniqueFd writer;
    Pipe() {
        std::array<int, 2> fds{};
        check(::pipe(fds.data()), "pipe");
        reader.reset(fds[0]);
        writer.reset(fds[1]);
        check(::fcntl(reader.get(), F_SETFD, FD_CLOEXEC), "fcntl");
        check(::fcntl(writer.get(), F_SETFD, FD_CLOEXEC), "fcntl");
    }
};

inline void write_all(int fd, std::span<const std::byte> bytes) {
    while (!bytes.empty()) {
        const auto written = ::write(fd, bytes.data(), bytes.size());
        if (written < 0 && errno == EINTR) continue;
        if (written < 0) fail("write");
        if (written == 0) throw std::runtime_error("write made no progress");
        bytes = bytes.subspan(static_cast<std::size_t>(written));
    }
}

// A clean EOF before a record returns false; EOF inside a record is an error.
inline bool read_exact(int fd, std::span<std::byte> bytes) {
    const auto expected = bytes.size();
    while (!bytes.empty()) {
        const auto received = ::read(fd, bytes.data(), bytes.size());
        if (received < 0 && errno == EINTR) continue;
        if (received < 0) fail("read");
        if (received == 0) {
            if (bytes.size() == expected) return false;
            throw std::runtime_error("Truncated pipe record");
        }
        bytes = bytes.subspan(static_cast<std::size_t>(received));
    }
    return true;
}

inline int wait_for(pid_t pid) {
    if (pid <= 0) throw std::invalid_argument("wait_for needs an owned child PID");
    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) fail("waitpid");
    }
    return status;
}

inline bool succeeded(int status) noexcept {
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

class ChildGroup {
public:
    explicit ChildGroup(std::size_t capacity) : exceptions_(std::uncaught_exceptions()) {
        children_.reserve(capacity); // Never allocate after a successful fork.
    }
    ~ChildGroup() {
        if (std::uncaught_exceptions() > exceptions_) terminate();
        for (const auto pid : children_) {
            if (pid <= 0) continue;
            int status = 0;
            while (::waitpid(pid, &status, 0) < 0 && errno == EINTR) {}
        }
    }
    ChildGroup(const ChildGroup&) = delete;
    ChildGroup& operator=(const ChildGroup&) = delete;

    template <typename Function>
    void spawn(Function&& work) {
        if (children_.size() == children_.capacity()) {
            throw std::length_error("ChildGroup capacity exceeded");
        }
        std::cout.flush();
        std::cerr.flush();
        const auto pid = ::fork();
        if (pid < 0) fail("fork");
        if (pid == 0) {
            int code = EXIT_SUCCESS;
            try { work(); }
            catch (const std::exception& error) {
                std::cerr << "Child error: " << error.what() << '\n';
                code = EXIT_FAILURE;
            }
            catch (...) { code = EXIT_FAILURE; }
            std::cout.flush();
            std::cerr.flush();
            ::_exit(code); // Do not run inherited parent owners/atexit handlers.
        }
        children_.push_back(pid);
    }

    bool wait_all() {
        bool success = true;
        for (auto& pid : children_) {
            if (pid <= 0) continue;
            const int status = wait_for(pid);
            pid = -1;
            success = succeeded(status) && success;
        }
        return success;
    }

    void terminate() noexcept {
        for (const auto pid : children_) if (pid > 0) ::kill(pid, SIGTERM);
    }

private:
    std::vector<pid_t> children_;
    int exceptions_;
};

class Mapping {
public:
    explicit Mapping(std::size_t size, int fd = -1) : size_(size) {
        address_ = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                          MAP_SHARED | (fd < 0 ? MAP_ANONYMOUS : 0), fd, 0);
        if (address_ == MAP_FAILED) fail("mmap");
    }
    ~Mapping() { ::munmap(address_, size_); }
    Mapping(const Mapping&) = delete;
    Mapping& operator=(const Mapping&) = delete;
    void* get() const noexcept { return address_; }

private:
    std::size_t size_;
    void* address_;
};

template <typename T>
class SharedObject {
public:
    explicit SharedObject(int fd = -1) : mapping_(sizeof(T), fd) {
        std::construct_at(get());
    }
    ~SharedObject() { std::destroy_at(get()); }
    T* get() const noexcept { return static_cast<T*>(mapping_.get()); }
    T* operator->() const noexcept { return get(); }

private:
    Mapping mapping_;
};

class Semaphore {
public:
    explicit Semaphore(unsigned int permits, const std::string& label = "permits") {
        const auto name = "/cpp_" + label + '_' + std::to_string(::getpid());
        value_ = ::sem_open(name.c_str(), O_CREAT | O_EXCL, 0600, permits);
        if (value_ == SEM_FAILED) fail("sem_open");
        // Forked children inherit the handle; no name remains to collide with
        // another run or leak if a process exits unexpectedly.
        if (::sem_unlink(name.c_str()) < 0) {
            const int error = errno;
            ::sem_close(value_);
            errno = error;
            fail("sem_unlink");
        }
    }
    ~Semaphore() { ::sem_close(value_); }
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    void lock() {
        while (::sem_wait(value_) < 0) if (errno != EINTR) fail("sem_wait");
    }
    void unlock() noexcept { ::sem_post(value_); }

private:
    sem_t* value_;
};

class TemporaryDirectory {
public:
    TemporaryDirectory() {
        path_ = (std::filesystem::temp_directory_path() / "cpp-process-XXXXXX").string();
        if (::mkdtemp(path_.data()) == nullptr) fail("mkdtemp");
    }
    ~TemporaryDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }
    TemporaryDirectory(const TemporaryDirectory&) = delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;
    std::filesystem::path path() const { return path_; }

private:
    std::string path_;
};

} // namespace posix_example
