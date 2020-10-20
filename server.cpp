#include "server.hpp"
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <csignal>
#include <sys/wait.h>
#include <algorithm>

using namespace HTTPServer;

Server::Server(int port, int n_threads)
: pool(n_threads)
{
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Cannot create socket: " << strerror(errno);
        exit(EXIT_FAILURE);
    }

    // Set socket to reuse, so that we can re-bind quickly
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        // Not a fatal error, just nice to have
        std::cerr << "Failed to set socket to reuse: " << strerror(errno);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;   // Accept any incoming messages
    addr.sin_port = htons(port);

    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port " << port << "\n" << strerror(errno);
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    close(fd);
}

void Server::serve() {
    if (listen(fd, MAX_CONNECTIONS) < 0) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    while(true) {
        // std::cout << "Waiting for new connection\n";

        int client_fd;
        sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);

        if ((client_fd = accept(fd, reinterpret_cast<sockaddr*>(&client_addr), &addrlen)) < 0) {
            // Child termination signal can interrupt accept call, so try again if that happens
            if (errno == EINTR)
                continue;
            std::cerr << "Failed to accept client connection" << strerror(errno);
            exit(EXIT_FAILURE);
        }

        pool.enqueue([this](int client_fd) {
            HTTPServer::Server::handleClient(client_fd);
        }, client_fd);
    }
}

void Server::handleClient(int client_fd) {
    handleRequests(client_fd);
    close(client_fd);
}

void Server::handleRequests(int client_fd) {
    int recv_length=0;
    char buffer[BUFFER_SIZE];

    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    std::string term = "\r\n\r\n";

    while(true) {
        recv_length = ::read(client_fd, buffer, BUFFER_SIZE);
        if (recv_length==0)
            break;
        else if (recv_length==-1) {
            if (errno==EINTR || errno==EAGAIN)
                continue;
            else {
                perror("Read error!");
                return;
            }
        }

        parseRequest(buffer, recv_length);
        if (write(client_fd, hello.c_str(), hello.length()) < 0) {
            perror("Write: ");
            return;
        }

        if (std::search(buffer, buffer+BUFFER_SIZE, term.begin(), term.end()) != buffer+BUFFER_SIZE)
            break;
    }
}

void Server::parseRequest(char* buffer, int length) {
    // Nothing here right now
    // std::cout << length << "\n" << buffer << std::endl;
}
