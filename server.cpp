#include "server.hpp"
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <csignal>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <algorithm>

using namespace HTTPServer;

Server::Server(int port)
{
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Cannot create socket: " << strerror(errno);
        exit(EXIT_FAILURE);
    }

    // Set socket to reuse, so that we can re-bind quickly
    int reuse = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        // Not a fatal error, just nice to have
        std::cerr << "Failed to set socket to reuse: " << strerror(errno);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;   // Accept any incoming messages
    addr.sin_port = htons(port);

    if (bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port " << port << "\n" << strerror(errno);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, MAX_CONNECTIONS) < 0) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    close(listen_fd);
    close(epfd);
}

void Server::serve() {

    epfd = epoll_create(1);

    // Add listening socket
    static epoll_event accept_event;
    accept_event.events = EPOLLIN;    // Monitor for new data
    accept_event.data.fd = listen_fd;        // Store the main socket for future use

    // Add fd to be monitored
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &accept_event) < 0) {
        perror("epoll_ctl EPOLL_CTL_ADD failed: ");
        exit(EXIT_FAILURE);
    }

    epoll_event events[MAX_EVENTS];

    while(true) {

        // Poll
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, MAX_WAITTIME_MS);
        if (nfds < 0) {
            perror("Epoll error: ");
            exit(EXIT_FAILURE);
        }

        for (int i=0; i<nfds; i++) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                // Listening socket is ready, New client
                sockaddr_in client_addr;
                socklen_t addrlen = sizeof(client_addr);
                int client_fd = accept(listen_fd, reinterpret_cast<sockaddr*>(&client_addr), &addrlen);
                if (client_fd < 0) {
                    perror("Error while accepting client connection!");
                    break;
                }

                // Make socket non-blocking
                int flags = fcntl(client_fd, F_GETFL, 0);
                if (flags==-1) {
                    perror("fcntl F_GETFL");
                    return;
                }

                if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
                    perror("fcntl F_SETFL O_NONBLOCK");

                // Create Handler for client
                handlers[client_fd] = std::make_unique<Handler>(Handler(client_fd));

                epoll_event ev{0};

                // Add new client to epoll
                ev.events = EPOLLOUT;
                ev.data.fd = client_fd;

                // Add to epoll
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
            }   // Adding new client finished
            else {
                StatusFD status;
                epoll_event ev{0};

                ev.data.fd = fd;

                // Handle a client
                if (events[i].events & EPOLLIN) {
                    // Ready for reading
                    status = handlers[fd]->readRequest();
                }
                else if (events[i].events & EPOLLOUT) {
                    // Ready for writing
                    status = handlers[fd]->sendResponse();
                }

                // ev.data.fd = client_fd;
                if (status.want_read)
                    ev.events |= EPOLLIN;
                if (status.want_write)
                    ev.events |= EPOLLOUT;

                if (ev.events == 0) {
                    // Close and delete
                    handlers[fd].reset();
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
                else if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
                    perror("epoll EPOLL_CTL_MOD");
                }
            }   // Handling existing client

        }   // Complete going through all file descriptors

    }   // Infinite loop ends (Well, actually not)

}
