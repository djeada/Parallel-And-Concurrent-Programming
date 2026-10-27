/**
 * C++20 + POSIX: bounded asynchronous TCP echo service.
 * A single poll event loop owns sockets and caps per-client output buffering.
 * Client/worker modes demonstrate stream framing and complete sends/receives;
 * this example is a socket protocol, not a distributed task dispatcher.
 * Compile: g++ -std=c++20 -pthread distributed_computing.cpp
 * Run: ./distributed_computing [server|worker|client] port
 */

#include "../common/posix.hpp"

#include <arpa/inet.h>
#include <array>
#include <poll.h>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <stdexcept>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

constexpr int BUFFER_SIZE = 1024;

// TCP is a byte stream: a successful send may write only part of the buffer.
bool send_all(int fd, const char* data, size_t size) {
    while (size > 0) {
        ssize_t bytes = send(fd, data, size, 0);
        if (bytes < 0 && errno == EINTR) continue;
        if (bytes <= 0) return false;
        data += bytes;
        size -= bytes;
    }
    return true;
}

ssize_t receive(int fd, char* data, size_t size) {
    ssize_t bytes;
    do {
        bytes = recv(fd, data, size, 0);
    } while (bytes < 0 && errno == EINTR);
    return bytes;
}

// A bounded poll event loop owns all client sockets. Each output buffer is
// capped; pausing reads supplies backpressure when a peer is slow to receive.
class Server {
public:
    explicit Server(int port) : socket_fd_(socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0)) {
        if (socket_fd_.get() < 0) posix_example::fail("socket");
        const int enabled = 1;
        posix_example::check(setsockopt(socket_fd_.get(), SOL_SOCKET, SO_REUSEADDR,
                                       &enabled, sizeof(enabled)), "setsockopt");
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons(port);
        posix_example::check(bind(socket_fd_.get(), reinterpret_cast<sockaddr*>(&address),
                                  sizeof(address)), "bind");
        posix_example::check(listen(socket_fd_.get(), SOMAXCONN), "listen");
        set_nonblocking(socket_fd_.get());
    }

    void start() {
        std::cout << "Server started\n";
        while (true) {
            std::vector<pollfd> events;
            events.push_back({socket_fd_.get(), static_cast<short>(clients_.size() < 64 ? POLLIN : 0), 0});
            for (const auto& client : clients_) {
                short requested = 0;
                if (!client.eof && client.pending.size() < buffer_limit) requested |= POLLIN;
                if (!client.pending.empty()) requested |= POLLOUT;
                events.push_back({client.fd.get(), requested, 0});
            }
            const int ready = poll(events.data(), events.size(), -1);
            if (ready < 0 && errno == EINTR) continue;
            posix_example::check(ready, "poll");
            // Process existing clients before accepting: appending can move the vector.
            for (std::size_t index = clients_.size(); index-- > 0;) {
                auto& client = clients_[index];
                const short happened = events[index + 1].revents;
                bool failed = happened & (POLLERR | POLLNVAL);
                if (!failed && happened & (POLLIN | POLLHUP)) {
                    std::array<char, BUFFER_SIZE> buffer{};
                    const auto room = buffer_limit - client.pending.size();
                    if (room != 0) {
                        const auto received = recv(client.fd.get(), buffer.data(), std::min(buffer.size(), room), 0);
                        if (received > 0) client.pending.append(buffer.data(), static_cast<std::size_t>(received));
                        else if (received == 0) client.eof = true;
                        else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) failed = true;
                    }
                }
                if (!failed && happened & POLLOUT) {
                    const auto sent = send(client.fd.get(), client.pending.data(), client.pending.size(), 0);
                    if (sent > 0) client.pending.erase(0, static_cast<std::size_t>(sent));
                    else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) failed = true;
                }
                if (failed || (client.eof && client.pending.empty())) {
                    clients_.erase(clients_.begin() + static_cast<std::ptrdiff_t>(index));
                }
            }
            if (events.front().revents & POLLIN) {
                posix_example::UniqueFd client(accept(socket_fd_.get(), nullptr, nullptr));
                if (client.get() < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) continue;
                    posix_example::fail("accept");
                }
                set_nonblocking(client.get());
                clients_.push_back({std::move(client), {}, false});
            }
        }
    }

private:
    static void set_nonblocking(int fd) {
        const int flags = fcntl(fd, F_GETFL);
        posix_example::check(flags, "fcntl get flags");
        posix_example::check(fcntl(fd, F_SETFL, flags | O_NONBLOCK), "fcntl nonblocking");
    }
    static constexpr std::size_t buffer_limit = 64 * 1024;
    struct Connection {
        posix_example::UniqueFd fd;
        std::string pending;
        bool eof;
    };
    posix_example::UniqueFd socket_fd_;
    std::vector<Connection> clients_;
};

class Worker {
public:
    Worker(const std::string& ip, int port) {
        socket_fd_.reset(socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0));
        if (socket_fd_.get() < 0) posix_example::fail("socket");

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        if (connect(socket_fd_.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            throw std::runtime_error("Connection failed");
        }
    }


    void run() {
        std::array<char, BUFFER_SIZE> buffer{};
        while (true) {
            ssize_t bytes = receive(socket_fd_.get(), buffer.data(), buffer.size());
            if (bytes <= 0) break;

            std::cout << "Task: ";
            std::cout.write(buffer.data(), bytes);
            std::cout << "\n";
            if (!send_all(socket_fd_.get(), buffer.data(), bytes)) break;
        }
    }

private:
    posix_example::UniqueFd socket_fd_;
};

class Client {
public:
    Client(const std::string& ip, int port) {
        socket_fd_.reset(socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0));
        if (socket_fd_.get() < 0) posix_example::fail("socket");

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        if (connect(socket_fd_.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            throw std::runtime_error("Connection failed");
        }
    }


    void run() {
        std::string input;
        std::array<char, BUFFER_SIZE> buffer{};

        while (true) {
            std::cout << "Enter task: ";
            if (!std::getline(std::cin, input)) break;
            // An empty send produces no bytes and therefore no echo response.
            if (input.empty()) continue;

            if (!send_all(socket_fd_.get(), input.data(), input.size())) {
                throw std::runtime_error("Sending task failed");
            }

            // The echo has exactly the task's length, possibly across many reads.
            std::string result;
            while (result.size() < input.size()) {
                ssize_t bytes = receive(socket_fd_.get(), buffer.data(),
                    std::min(buffer.size(), input.size() - result.size()));
                if (bytes <= 0) {
                    throw std::runtime_error("Connection closed before the full reply");
                }
                result.append(buffer.data(), bytes);
            }

            std::cout << "Result: " << result << "\n";
        }
    }

private:
    posix_example::UniqueFd socket_fd_;
};

int main(int argc, char* argv[]) {
    // Report failed sends through their return value instead of terminating
    // every connection when a single peer disconnects during a write.
    std::signal(SIGPIPE, SIG_IGN);
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " [server|worker|client] port\n";
        return 1;
    }

    const std::string role = argv[1];
    try {
        const int port = std::stoi(argv[2]);
        if (port < 1 || port > 65535) throw std::invalid_argument("Port must be between 1 and 65535");
        if (role == "server") {
            Server server(port);
            server.start();
        } else if (role == "worker") {
            Worker worker("127.0.0.1", port);
            worker.run();
        } else if (role == "client") {
            Client client("127.0.0.1", port);
            client.run();
        } else {
            std::cerr << "Unknown role: " << role << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
