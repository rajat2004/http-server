#include "server.hpp"
#include <iostream>

const int SRV_PORT = 8080;

int main(int argc, char * argv[]) {
    int portno = SRV_PORT;

    if (argc > 1) {
        portno = atoi(argv[1]);
    }

    Server server(portno);
    server.serve();
}
