#include "server.hpp"
#include <string>

void handle_request(int client_fd) {
    int recv_length = 0;
    char buffer[BUFFER_SIZE];

    recv_length = read(client_fd, buffer, BUFFER_SIZE);
    std::cout << buffer << "\n" << recv_length << std::endl;

    std::string hello = "HTTP/1.1 200 OK\n\nHello, World!";
    write(client_fd, hello.c_str(), hello.length());
}

int main() {
    // Create a server socket
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    // Set socket to reuse, so that we can re-bind quickly
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        // Not a fatal error, just nice to have
        perror("Failed to set socket to reuse");
    }

    // Struct for binding socket to port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;   // Accept any incoming messages
    address.sin_port = htons(PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        perror("Failed to bind to port");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 8) < 0) {     // Some randon number for now, should be max connections
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    while(true) {
        std::cout << "Waiting for new connection\n";

        int client_fd;
        sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);

        if ((client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &addrlen)) < 0) {
            perror("Failed to accept client connection");
            exit(EXIT_FAILURE);
        }
        handle_request(client_fd);
        close(client_fd);
    }

    return 0;
}
