/**
 * ⚠️  ANTIPATTERN — Detached-Thread-Per-Connection Server
 *
 * This file shows a common but problematic HTTP server pattern where
 * each incoming connection is dispatched to a detached std::thread.
 *
 * Problems with this approach:
 *   1. Detached threads cannot be joined — no graceful shutdown.
 *      When main() returns while handlers are still running, the process
 *      terminates and threads are abandoned mid-request.
 *   2. No resource limit — a burst of connections spawns an unbounded
 *      number of threads, potentially exhausting the system.
 *   3. Exceptions in detached threads call std::terminate() with no
 *      way for the server to recover or log the failure.
 *   4. The <future> header is included but std::async is never used;
 *      the name "async server" is misleading.
 *
 * The correct approach is a thread pool (see thread_pool_futures.cpp)
 * or an event-loop framework such as Boost.Asio.
 *
 * Note: Run with:  ./async_server
 *       Test with: curl http://localhost:8080
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
