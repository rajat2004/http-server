#include "server.hpp"
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <csignal>
#include <sys/wait.h>


Server::Server(int port) {
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

void Server::serve() {
    if (listen(fd, MAX_CONNECTIONS) < 0) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    // Signal handler to prevent zombies and using up all resources
    // signal(SIGCHLD, Server::signalHandler);

    struct sigaction sa;
    sa.sa_handler = Server::signalHandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, 0);
    // sigaction()

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

        handleClient(client_fd);
        close(client_fd);
    }
}

void Server::signalHandler(int signum) {
    int status, cpid;

    while (true) {
        cpid = waitpid(-1, &status, WNOHANG);
        if (cpid<0) {
            if (errno!=ECHILD)
                std::cerr << "Error while waiting for child process: " << strerror(errno) << std::endl;

            return;
        }
        else if (cpid==0)
            break;          // No more zombies
    }

    // cpid = wait(&status);
    // std::cout << "Child " << cpid << " exited with status " << status << std::endl;
}

void Server::handleClient(int client_fd) {
    // New child to handle the client
    int pid = fork();

    if (pid<0) {
        perror("Forking error!");
        return;
    }

    // Child
    if (pid==0) {
        // Close server socket copy, we don't need it
        close(fd);
        handleRequest(client_fd);
        close(client_fd);
        exit(EXIT_SUCCESS); // Child finishes its work
    }
    else {
        close(client_fd);   // Again, no need of this in parent
        num_clients++;
        std::cout << num_clients << std::endl;
    }

}

void Server::handleRequest(int client_fd) {
    int recv_length = 0;
    char buffer[BUFFER_SIZE];

    recv_length = read(client_fd, buffer, BUFFER_SIZE);
    parseRequest(buffer, recv_length);

    std::string hello = "HTTP/1.1 200 OK\n\nHello, World!";
    write(client_fd, hello.c_str(), hello.length());
}

void Server::parseRequest(char* buffer, int length) {
    // Nothing here right now
    // std::cout << length << "\n" << buffer << std::endl;
}
