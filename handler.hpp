#ifndef HANDLER_H
#define HANDLER_H

#include <string>
#include <unordered_map>

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
    // TODO: Move to separate parser class, with caching as well
    void parseRequest();
    void createResponse();

    int client;
    std::string read_req, response;
    char buffer[BUFFER_SIZE];
    std::unordered_map<std::string, std::string> req_headers;

    // Cache for files, might be better to make this a single cache for all handlers
    std::unordered_map<std::string, std::string> file_cache;
};


}   // namespace HTTPServer

#endif // HANDLER_H
