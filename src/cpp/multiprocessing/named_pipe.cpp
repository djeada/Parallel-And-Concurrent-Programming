/**
 * C++20 + POSIX: FIFO ownership and complete byte-stream transfers.
 * The self-contained demo uses a private temporary directory. For unrelated
 * processes, pass the same explicit path to both reader and writer modes.
 * Compile: g++ -std=c++20 -pthread named_pipe.cpp
 * Run: ./named_pipe
 * Or: ./named_pipe reader /tmp/my_fifo  (then writer /tmp/my_fifo elsewhere)
 */
#include "../common/posix.hpp"

#include <array>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <sys/stat.h>

void create_fifo(const std::string& path) {
    if (mkfifo(path.c_str(), 0600) < 0) {
        if (errno != EEXIST) posix_example::fail("mkfifo");
        struct stat status{};
        posix_example::check(lstat(path.c_str(), &status), "lstat");
        if (!S_ISFIFO(status.st_mode)) throw std::runtime_error("Existing path is not a FIFO");
    }
}

void run_writer(const std::string& path) {
    auto fd = posix_example::open_file(path.c_str(), O_WRONLY);
    for (int index = 1; index <= 5; ++index) {
        const auto line = "Message " + std::to_string(index) + '\n';
        posix_example::write_all(fd.get(), std::as_bytes(std::span(line)));
        std::cout << "[writer] sent: " << line;
    }
}

void run_reader(const std::string& path) {
    auto fd = posix_example::open_file(path.c_str(), O_RDONLY);
    std::array<char, 256> buffer{};
    while (true) {
        const auto bytes = read(fd.get(), buffer.data(), buffer.size());
        if (bytes < 0 && errno == EINTR) continue;
        if (bytes < 0) posix_example::fail("read FIFO");
        if (bytes == 0) break;
        std::cout.write(buffer.data(), bytes);
    }
    std::cout << "[reader] EOF, done\n";
}

int main(int argc, char* argv[]) try {
    // A closed reader produces an EPIPE error, handled by write_all.
    std::signal(SIGPIPE, SIG_IGN);
    if (argc == 3) {
        const std::string_view mode(argv[1]);
        const std::string path(argv[2]);
        if (mode != "reader" && mode != "writer") throw std::invalid_argument("Unknown FIFO mode");
        create_fifo(path);
        if (mode == "reader") {
            run_reader(path);
            posix_example::check(unlink(path.c_str()), "unlink FIFO");
        } else {
            run_writer(path);
        }
        return 0;
    }
    if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " [reader|writer path]\n";
        return 1;
    }
    posix_example::TemporaryDirectory directory;
    const auto path = (directory.path() / "messages.fifo").string();
    create_fifo(path);
    posix_example::ChildGroup children(2);
    children.spawn([&] { run_writer(path); });
    children.spawn([&] { run_reader(path); });
    if (!children.wait_all()) return 1;
    std::cout << "FIFO demo complete\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
