#include <sys/socket.h>
#include <netinet/in.h>

const int BUFFER_SIZE = 10000;  // Some arbitrary large no

const int MAX_CONNECTIONS = 1e5;

class Server {
public:
    Server(int port);

    void serve();

private:
    int fd = -1;
    sockaddr_in addr {};
    int num_clients = 0;

    void handleClient(int client_fd);
    void handleRequest(int client_fd);
    void parseRequest(char* buffer, int length);

    static void signalHandler(int signum);
};
