#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

const int SERVER_PORT = 8080;
const int BUFFER_SIZE = 4096;
const int CLIENTS = 1000;
const int REQ_PER_CLIENT = 100;

static const std::string req = "GET / HTTP/1.1\nHost: localhost:8080\r\n\r\n";

using namespace std;
using namespace chrono;

void createClient(int, int);

int main(int argc, char * argv[]) {

    int n_clients=CLIENTS, reqs_per_client=REQ_PER_CLIENT;

    if (argc > 1)
        n_clients = stoi(argv[1]);
    if (argc > 2)
        reqs_per_client = stoi(argv[2]);

    vector<thread> clients;

    // Measure the amount of time taken for everything
    auto start = high_resolution_clock::now();

    for(int i=0; i<n_clients; i++)
        clients.emplace_back(std::thread([reqs_per_client](int id) {
            createClient(id, reqs_per_client);
        }, i));

    for(auto &client:clients)
        client.join();

    auto end = chrono::high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);
	cout << "Duration = " << duration.count() / 1000.0f << "s for " << n_clients << " clients, "
            << reqs_per_client << " reqs per client" << endl;

    return 0;
}

void createClient(int id, int reqs_per_client) {
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

    char buffer[BUFFER_SIZE];

    for (int i=0; i<reqs_per_client; i++) {
        send(client_fd, req.c_str(), req.length(), 0);
        // std::cout << "Sent message from client " << id << "\n";

        int recv_length = read(client_fd, buffer, BUFFER_SIZE);
        // std::cout << "Message from server: " << buffer << "\n" << recv_length << std::endl;
    }

    close(client_fd);
}
