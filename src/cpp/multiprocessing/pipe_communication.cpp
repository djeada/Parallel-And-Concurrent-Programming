/**
 * C++20 + POSIX: owned pipe endpoints and complete records.
 * Pipe owns both descriptors; close unused endpoints before waiting for EOF.
 * read_exact/write_all handle short transfers, EINTR, and truncated records.
 * This native-int protocol is for related processes on the same machine.
 * Compile: g++ -std=c++20 -pthread pipe_communication.cpp
 */
#include "../common/posix.hpp"

#include <iostream>
#include <span>

int main() try {
    posix_example::Pipe pipe;
    posix_example::ChildGroup children(1);
    children.spawn([&] {
        pipe.writer.reset();
        int item = 0;
        while (posix_example::read_exact(pipe.reader.get(),
                                        std::as_writable_bytes(std::span(&item, 1)))) {
            std::cout << "Consumer: received " << item << '\n';
        }
    });
    pipe.reader.reset();
    for (int item = 1; item <= 5; ++item) {
        posix_example::write_all(pipe.writer.get(), std::as_bytes(std::span(&item, 1)));
        std::cout << "Producer: sending " << item << '\n';
    }
    pipe.writer.reset(); // EOF must be visible before waiting for the consumer.
    if (!children.wait_all()) return 1;
    std::cout << "Done\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
