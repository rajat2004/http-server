CXX = g++
CFLAGS = -Wall -Wextra

SERVER_SRC = server.cpp server.hpp
CLIENT_SRC = client.cpp

all: server client

server: $(SERVER_SRC)
	$(CXX) $(CFLAGS) -o $@ $(SERVER_SRC)

client: $(CLIENT_SRC)
	$(CXX) $(CFLAGS) -o $@ $(CLIENT_SRC)

clean:
	rm -f server client
