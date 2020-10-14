CXX = g++
CFLAGS = -Wall -Wextra

SERVER_SRC = http_server.cpp server.cpp server.hpp
CLIENT_SRC = client.cpp

all: http_server client

http_server: $(SERVER_SRC)
	$(CXX) $(CFLAGS) -o $@ $(SERVER_SRC)

client: $(CLIENT_SRC)
	$(CXX) $(CFLAGS) -o $@ $(CLIENT_SRC)

clean:
	rm -f http_server client
