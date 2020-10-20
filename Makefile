CXX = g++
CFLAGS = -O2 -std=c++17 -Wall -Wextra -Wno-unused -pthread

all: server client

server: server_main.cpp server.cpp server.hpp thread_pool.hpp
	$(CXX) $(CFLAGS) -o $@ $^

client: client.cpp
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f server client
