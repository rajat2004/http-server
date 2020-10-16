#ifndef HANDLER_H
#define HANDLER_H

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>

const int BUFFER_SIZE = 4096;

namespace HTTPServer {

struct StatusFD {
    bool want_read = false;
    bool want_write = false;

    StatusFD() {}

    StatusFD(bool read, bool write)
    : want_read(read), want_write(write)
    {}
};

const StatusFD Read{true, false};
const StatusFD Write{false, true};
const StatusFD ReadWrite{true, true};
const StatusFD Close{false, false};

class Handler {
public:
    Handler(int client_fd);
    ~Handler();

    StatusFD readRequest();

    StatusFD sendResponse();

private:
    bool getRequest();
    void createResponse();

    int client;
    std::string read;
    char buffer[BUFFER_SIZE];
};


}   // HTTPServer

#endif // HANDLER_H
