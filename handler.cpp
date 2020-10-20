#include "handler.hpp"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>

using namespace HTTPServer;

// For testing
static const std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

Handler::Handler(int client_fd)
: client(client_fd)
{
}

Handler::~Handler()
{
    // close(client);
}

StatusFD Handler::readRequest()
{
    int recv_length=0;

    static const std::string term = "\r\n\r\n";

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
    read_req.append(buffer);

    if (std::search(read_req.begin(), read_req.end(), term.begin(), term.end()) != read_req.end()) {
        // Found terminating character, now wait for writing
        // TODO: Move parsing, file handling to different class
        //       With a cache for files already read
        parseRequest();
        createResponse();

        return Write;
    }
    else
        // More to read
        return Read;
}

StatusFD Handler::sendResponse()
{
    // response = hello;

    if (write(client, response.c_str(), response.length()) < 0) {
        perror("Write failed: ");
        return Close;
    }

    return Read;
}

void Handler::parseRequest()
{
    // std::cout << read_req << std::endl;

    // Only parsing the first line for now
    // TODO: Parse all lines
    std::string line_term = "\r\n";
    auto line_end = std::search(read_req.begin(), read_req.end(), line_term.begin(), line_term.end());

    // Find request type
    auto last_pos=read_req.begin(), pos=std::find(last_pos, line_end, ' ');
    req_headers["Type"] = std::string(last_pos, pos);
    last_pos = pos;
    last_pos++;

    pos = std::find(last_pos, line_end, ' ');
    req_headers["Path"] = std::string(last_pos, pos);
    last_pos = pos;
    last_pos++;

    req_headers["Version"] = std::string(last_pos, line_end);
}

void Handler::createResponse()
{
    response = "HTTP/1.1 ";

    if (req_headers["Type"]!="GET") {
        // Ony GET for now
        response.append("501 Not Implemented");
        return;
    }

    std::string path = "site/";
    if (req_headers["Path"]=="/")
        path.append("index.html");
    else
        path.append(req_headers["Path"]);

    // std::cout << path << std::endl;

    // Check if already in cache
    if (file_cache.find(path)!=file_cache.end()) {
        response = file_cache[path];
        return;
    }

    std::string line, res_content;
    std::ifstream file(path);

    // Can be done using std::filesystem, but seemed too much to include for one task (without even considering the compiler version headaches)
    if (!file.good()) {
        response.append("404 Not Found");
        return;
    }

    std::string res_header = "200 OK\r\nContent-Type: text/html\nContent-Length: ";

    int fsize=0;

    if (file.is_open()) {
        while(getline(file, line)) {
            fsize += line.length();
            res_content.append(line);
        }

        file.close();
    }

    response.append(res_header + std::to_string(fsize) + "\n\n");
    response.append(res_content);

    // Store response in cache for future
    file_cache[path] = response;
}
