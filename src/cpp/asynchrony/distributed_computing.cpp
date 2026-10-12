/**
 * Distributed Computing Pattern
 *
 * This example demonstrates a basic distributed computing pattern
 * with a server coordinating work among multiple workers.
 *
 * Key concepts:
 * - Server accepts connections and distributes tasks
 * - Workers connect and process assigned work
 * - Clients submit work and receive results
 * - Socket-based communication between components
 *
 * Run as:
 *   ./distributed_computing server 3000
 *   ./distributed_computing worker 3000  (in another terminal)
 *   ./distributed_computing client 3000  (in another terminal)
 */

#include <arpa/inet.h>
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

class Server {
public:
    explicit Server(int port) {
        socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd_ < 0) {
            throw std::runtime_error("Socket creation failed");
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(socket_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            throw std::runtime_error("Bind failed");
        }
    }

    ~Server() { close(socket_fd_); }

    void start() {
        listen(socket_fd_, SOMAXCONN);
        std::cout << "Server started\n";

        while (true) {
            sockaddr_in client_addr{};
            socklen_t addr_len = sizeof(client_addr);
            int client_fd = accept(socket_fd_,
                                   reinterpret_cast<sockaddr*>(&client_addr),
                                   &addr_len);
            if (client_fd >= 0) {
                std::thread(&Server::handle_client, this, client_fd).detach();
            }
        }
    }

private:
    void handle_client(int client_fd) {
        char buffer[BUFFER_SIZE];
        while (true) {
            ssize_t bytes = receive(client_fd, buffer, sizeof(buffer));
            if (bytes <= 0) break;

            std::cout << "Received: ";
            std::cout.write(buffer, bytes);
            std::cout << "\n";
            if (!send_all(client_fd, buffer, bytes)) break;  // Echo back
        }
        close(client_fd);
    }

    int socket_fd_;
};

class Worker {
public:
    Worker(const std::string& ip, int port) {
        socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        if (connect(socket_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            throw std::runtime_error("Connection failed");
        }
    }

    ~Worker() { close(socket_fd_); }

    void run() {
        char buffer[BUFFER_SIZE];
        while (true) {
            ssize_t bytes = receive(socket_fd_, buffer, sizeof(buffer));
            if (bytes <= 0) break;

            std::cout << "Task: ";
            std::cout.write(buffer, bytes);
            std::cout << "\n";
            if (!send_all(socket_fd_, buffer, bytes)) break;
        }
    }

private:
    int socket_fd_;
};

class Client {
public:
    Client(const std::string& ip, int port) {
        socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        if (connect(socket_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            throw std::runtime_error("Connection failed");
        }
    }

    ~Client() { close(socket_fd_); }

    void run() {
        std::string input;
        char buffer[BUFFER_SIZE];

        while (true) {
            std::cout << "Enter task: ";
            if (!std::getline(std::cin, input)) break;
            // An empty send produces no bytes and therefore no echo response.
            if (input.empty()) continue;

            if (!send_all(socket_fd_, input.data(), input.size())) {
                throw std::runtime_error("Sending task failed");
            }

            // The echo has exactly the task's length, possibly across many reads.
            std::string result;
            while (result.size() < input.size()) {
                ssize_t bytes = receive(socket_fd_, buffer,
                    std::min(sizeof(buffer), input.size() - result.size()));
                if (bytes <= 0) {
                    throw std::runtime_error("Connection closed before the full reply");
                }
                result.append(buffer, bytes);
            }

            std::cout << "Result: " << result << "\n";
        }
    }

private:
    int socket_fd_;
};

int main(int argc, char* argv[]) {
    // Report failed sends through their return value instead of terminating
    // every connection when a single peer disconnects during a write.
    std::signal(SIGPIPE, SIG_IGN);
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " [server|worker|client] port\n";
        return 1;
    }

    std::string role = argv[1];
    int port = std::stoi(argv[2]);

    try {
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
