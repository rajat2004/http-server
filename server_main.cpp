#include "server.hpp"
#include <iostream>

using namespace HTTPServer;

const int SRV_PORT = 8080;

int main(int argc, char * argv[]) {
    int portno=SRV_PORT, n_threads=std::thread::hardware_concurrency();

    if (argc > 1)
        portno = atoi(argv[1]);
    if (argc > 2)
        n_threads = atoi(argv[2]);

    Server server(portno, n_threads);
    server.serve();
}
