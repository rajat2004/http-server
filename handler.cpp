#include "handler.hpp"

using namespace HTTPServer;

Handler::Handler(int client_fd)
: client(client_fd)
{
}

Handler::~Handler()
{
    // close(client);
}

StatusFD Handler::readRequest() {
    int recv_length=0;

    std::string term = "\r\n\r\n";

    recv_length = recv(client, buffer, BUFFER_SIZE, 0);
    if (recv_length==0)
        // Nothing more to read
        return Close;
    else if (recv_length==-1) {
        if (errno!=EINTR && errno!=EAGAIN)
            perror("Read error!");

        return Close;
    }

    // std::cout << buffer << std::endl;
    read.append(buffer);

    return Write;
}

StatusFD Handler::sendResponse() {
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    int write_len = write(client, hello.c_str(), hello.length());
    if (write_len<1) {
        perror("Write failed: ");
        return Close;
    }

    return Read;
}
