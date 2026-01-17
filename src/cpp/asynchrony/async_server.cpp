/**
 * Async HTTP Server
 *
 * This example demonstrates a simple HTTP server that handles
 * each client connection asynchronously using std::async.
 *
 * Key concepts:
 * - Main loop accepts connections synchronously
 * - Each connection spawns an async task for handling
 * - Enables concurrent client handling
 * - Non-blocking server architecture
 *
 * Note: This is a minimal example. Production servers need
 * proper error handling, timeouts, and resource limits.
 */

#include <arpa/inet.h>
#include <cstring>
#include <future>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

constexpr int PORT = 8080;

class AsyncServer {
public:
    explicit AsyncServer(int port) : port_(port), server_fd_(-1) {}

    bool start() {
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ < 0) {
            std::cerr << "Failed to create socket\n";
            return false;
        }

        int opt = 1;
        setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            std::cerr << "Failed to bind\n";
            return false;
        }

        if (listen(server_fd_, SOMAXCONN) < 0) {
            std::cerr << "Failed to listen\n";
            return false;
        }

        std::cout << "Server listening on port " << port_ << "\n";
        return true;
    }

    void run() {
        while (true) {
            sockaddr_in client_addr{};
            socklen_t addr_len = sizeof(client_addr);

            int client_fd = accept(server_fd_,
                                   reinterpret_cast<sockaddr*>(&client_addr),
                                   &addr_len);
            if (client_fd < 0) {
                std::cerr << "Accept failed\n";
                continue;
            }

            // Handle client asynchronously (detached - fire and forget)
            std::thread([this, client_fd]() {
                handle_client(client_fd);
            }).detach();
        }
    }

private:
    void handle_client(int client_fd) {
        const char* response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";

        send(client_fd, response, strlen(response), 0);
        close(client_fd);
    }

    int port_;
    int server_fd_;
};

int main() {
    AsyncServer server(PORT);

    if (!server.start()) {
        return 1;
    }

    server.run();
    return 0;
}
