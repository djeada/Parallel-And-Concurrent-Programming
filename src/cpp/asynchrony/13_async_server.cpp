#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <future>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

constexpr int PORT = 3000;

class HTTPServer {
public:
    HTTPServer(int port) : port_(port) {}

    void start() {
        server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_ == -1) {
            std::cerr << "Failed to create socket" << std::endl;
            return;
        }

        sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port_);
        server_address.sin_addr.s_addr = INADDR_ANY;

        if (bind(server_socket_, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == -1) {
            std::cerr << "Failed to bind socket" << std::endl;
            return;
        }

        if (listen(server_socket_, SOMAXCONN) == -1) {
            std::cerr << "Failed to listen on socket" << std::endl;
            return;
        }

        std::cout << "Server listening on port " << port_ << std::endl;

        while (true) {
            sockaddr_in client_address;
            socklen_t client_address_size = sizeof(client_address);
            int client_socket = accept(server_socket_, reinterpret_cast<sockaddr*>(&client_address), &client_address_size);

            if (client_socket == -1) {
                std::cerr << "Failed to accept connection" << std::endl;
                continue;
            }

            std::future<void> client_future = std::async(std::launch::async, &HTTPServer::handle_client, this, client_socket);
        }

        close(server_socket_);
    }

private:
    void handle_client(int client_socket) {
        const char* http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 12\r\n"
            "\r\n"
            "Hello, World!";

        send(client_socket, http_response, strlen(http_response), 0);
        close(client_socket);
    }

    int port_;
    int server_socket_;
};

int main() {
    HTTPServer server(PORT);
    server.start();
    return 0;
}
