#ifndef SERVER_H
#define SERVER_H

// #include "thread_pool.hpp"
#include "handler.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include <memory>
#include <vector>

const int MAX_CONNECTIONS = 1e5;
const int MAX_EVENTS = 1e4;         // 1024 per process, so seems like a good limit
const int MAX_WAITTIME_MS = 1e3;

namespace HTTPServer {

class Server {
public:
    Server(int port);
    ~Server();

    void serve();

private:
    int listen_fd = -1;
    int epfd = -1;
    sockaddr_in addr {};
    int num_clients = 0;

    std::unique_ptr<Handler> handlers[MAX_EVENTS];

    void handleClient(int client_fd);
    void handleRequests(int client_fd);
    void parseRequest(char* buffer, int length);
};

} // namespace HTTPServer

#endif // SERVER_H
