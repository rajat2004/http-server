#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>

const int SERVER_PORT = 8080;
const int BUFFER_SIZE = 10000;

int main() {
    int client_fd;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(client_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    std::string hello = "Hello World from client!";

    send(client_fd, hello.c_str(), hello.length(), 0);
    std::cout << "Sent message from client\n";

    char buffer[BUFFER_SIZE];
    int recv_length = read(client_fd, buffer, BUFFER_SIZE);
    std::cout << "Message from server: " << buffer << "\n" << recv_length << std::endl;

    close(client_fd);

    return 0;
}
