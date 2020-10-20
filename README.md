# HTTP Server in C++

Basic HTTP server written from scratch in C++ without any external libraries.

To build -

```shell
make
```

Tested using GCC 7.5.0 on Ubuntu 18.04

This creates a binary `server`, execute and open a browser and type `localhost:8080` to view an example page stored in `site/`.
To change the port no, pass the port as an argument, e.g. `./server 9090`

Currently only supports `GET` request with plain HTML test documents. In browser, on clicking the link, the webpage doesn't change. This is due to the very minimal parsing currently implemented, press `Ctrl + Shift + R` to refresh.

For testing, use the `client` binary. It can take 2 arguments - No. of clients, & No. of requests per client. E.g.

```shell
$ ./client 100 10
Duration = 0.037s for 100 clients, 10 reqs per client
```

I tried a few different approaches, can be seen on the specific branch linked below -

1. [Multi-Processing](https://github.com/rajat2004/http-server/tree/multiprocessing) - Creating a new process for every request or client
2. [Multi-Threading](https://github.com/rajat2004/http-server/tree/multithreading) - Separate thread for each request, later on using a Threadpool to minimize thread creation and destroy.
3. Event-driven (Asynchronous I/O) (This branch) - Single-threaded server which polls sockets (using `epoll`) and reads & writes when ready.

Note that directly benchmarking and comparing them isn't possible currently, since the Async I/O has features like parsing the HTTP request, doing file reading and sending. The others receive and send a fixed message only.

Some simple benchmarks from testing are present in [`benchmarks.md`](benchmarks.md)

There are several limitations currently, apart from the HTTP 1.1 standard. Linux has a soft limit of 1024 files per process, haven't played around with that right now. It can be worked around by having multiple processes for clients with threads inside them, and similarly for server with possibly different sockets binding to the same port.

Other improvements like a single cache for the server for storing files (currently only per client), threading with each handling multiple clients, etc. are also doable.

The above improvements, along with implementing some advanced thread-pooling techniques, etc. are a few things which I'm thinking of trying later on.

There are a lot of articles, blog posts, etc. dealing with the above concepts, I've listed down just a few (in no particular order) which I found particularly useful in explaining concepts, etc.

- [Medium: HTTP Server: Everything you need to know to Build a simple HTTP server from scratch](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)

- [Let’s Build A Web Server](https://ruslanspivak.com/lsbaws-part1/) - Awesome 3 part detailed series

- [Three ways to web server concurrency](https://www.linuxjournal.com/content/three-ways-web-server-concurrency?page=0,0)

- [Code Review: Simple Multi-Threaded Web Server](https://codereview.stackexchange.com/questions/122043/simple-multi-threaded-web-server) - Not an article, but has an excellent discussion on C++ features, etc.

- [The method to epoll's madness](https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642)

- [Concurrent Servers: Part 3 Event Driven](https://eli.thegreenplace.net/2017/concurrent-servers-part-3-event-driven/)

Searching for the original C10K problem and the currently relevant C10M problem will give some fascinating articles on this subject, and the approaches used to deal with them.
