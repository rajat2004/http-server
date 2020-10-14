# HTTP Server in C++

HHTP server written from scratch in C++ without any external libraries such as Boost/asio, etc. Currently aims to support HTTP/1.1 standard with concurrent connections

To build -

```shell
make
```

This creates a binary `http_server`, execute and open browser and type `localhost:8080` to view the `Hello World!` page.
To change the port no, pass the port as an argument e.g. `./http_server 9090`

`client.py` to test max no. of connections and requests. See `client.py -h` for options available.
