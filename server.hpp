#ifndef SERVER_H
#define SERVER_H

#include "thread_pool.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

const int BUFFER_SIZE = 4096;
const int MAX_CONNECTIONS = 1e5;

namespace HTTPServer {

class Server {
public:
    Server(int port, int n_threads=std::thread::hardware_concurrency());
    ~Server();

    void serve();

private:
    int fd = -1;
    sockaddr_in addr {};
    int num_clients = 0;

    ThreadPool pool;

    void handleClient(int client_fd);
    void handleRequests(int client_fd);
    void parseRequest(char* buffer, int length);
};

} // namespace HTTPServer

#endif // SERVER_H
