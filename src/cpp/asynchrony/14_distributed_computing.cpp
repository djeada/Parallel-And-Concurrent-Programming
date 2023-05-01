#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

const int PORT = 3000;
const int BUFFER_SIZE = 1024;

class Server {
public:
  Server(int port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
      std::cerr << "Error creating socket." << std::endl;
      exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
      std::cerr << "Error binding socket." << std::endl;
      exit(1);
    }
  }

  ~Server() { close(server_socket); }

  void start() {
    if (listen(server_socket, 1) < 0) {
      std::cerr << "Error listening." << std::endl;
      exit(1);
    }

    while (true) {
      sockaddr_in client_addr;
      socklen_t client_addr_size = sizeof(client_addr);
      int client_socket =
          accept(server_socket, (sockaddr *)&client_addr, &client_addr_size);
      if (client_socket < 0) {
        std::cerr << "Error accepting connection." << std::endl;
        exit(1);
      }

      std::thread worker_thread(&Server::handle_connection, this,
                                client_socket);
      worker_thread.detach();
    }
  }

private:
  int server_socket;
  sockaddr_in server_addr;

  void handle_connection(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
      std::memset(buffer, 0, BUFFER_SIZE);
      int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
      if (bytes_received <= 0) {
        break;
      }

      std::cout << "Client: " << buffer << std::endl;
      send(client_socket, buffer, bytes_received, 0);
    }

    close(client_socket);
  }
};

class Worker {
public:
  Worker(const std::string &server_ip, int port) {
    worker_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (worker_socket < 0) {
      std::cerr << "Error creating socket." << std::endl;
      exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_addr.sin_port = htons(port);
  }

  ~Worker() { close(worker_socket); }

  void start() {
    if (connect(worker_socket, (sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
      std::cerr << "Error connecting to server." << std::endl;
      exit(1);
    }

    char buffer[BUFFER_SIZE];
    while (true) {
      std::memset(buffer, 0, BUFFER_SIZE);
      int bytes_received = recv(worker_socket, buffer, BUFFER_SIZE, 0);
      if (bytes_received <= 0) {
        break;
      }

      std::cout << "Received task: " << buffer << std::endl;
      send(worker_socket, buffer, bytes_received, 0);
    }
  }

private:
  int worker_socket;
  sockaddr_in server_addr;
};

class Client {
public:
  Client(const std::string &server_ip, int port) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
      std::cerr << "Error creating socket." << std::endl;
      exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_addr.sin_port = htons(port);
  }

  ~Client() { close(client_socket); }

  void start() {
    if (connect(client_socket, (sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
      std::cerr << "Error connecting to server." << std::endl;
      exit(1);
    }

    std::string message;
    char buffer[BUFFER_SIZE];
    while (true) {
      std::cout << "Enter task: ";
      std::getline(std::cin, message);

      send(client_socket, message.c_str(), message.length(), 0);

      std::memset(buffer, 0, BUFFER_SIZE);
      int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
      if (bytes_received <= 0) {
        break;
      }

      std::cout << "Received result: " << buffer << std::endl;
    }
  }

private:
  int client_socket;
  sockaddr_in server_addr;
};

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " [server|worker|client] [port]"
              << std::endl;
    return 1;
  }

  std::string role = argv[1];
  int port = std::stoi(argv[2]);

  if (role == "server") {
    Server server(port);
    server.start();
  } else if (role == "worker") {
    Worker worker("127.0.0.1", port);
    worker.start();
  } else if (role == "client") {
    Client client("127.0.0.1", port);
    client.start();
  } else {
    std::cerr << "Invalid role: " << role << std::endl;
    return 1;
  }

  return 0;
}
