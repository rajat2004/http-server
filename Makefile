CXX = g++
CFLAGS = -O2 -std=c++17 -Wall -Wextra -pthread

all: server client

server: server_main.cpp server.o handler.o
	$(CXX) $(CFLAGS) -o $@ $^

server.o: server.cpp server.hpp handler.o
	$(CXX) $(CFLAGS) -c -o $@ server.cpp

handler.o: handler.cpp handler.hpp
	$(CXX) $(CFLAGS) -c -o $@ handler.cpp

client: client.cpp
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o server client
