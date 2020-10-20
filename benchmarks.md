### Server benchmarks

Values from some simple testing done. Not at all exhaustive.

- Multiprocessing
Duration = 0.443s for 100 clients, 100 reqs per client
Duration = 2.339s for 1000 clients, 100 reqs per client

- Multithreading
    - (Default threadpool - hardware concurreny)
    Duration = 0.198s for 100 clients, 100 reqs per client
    Duration = 2.584s for 1000 clients, 100 reqs per client

    - More threads (50)
    Duration = 0.216s for 100 clients, 100 reqs per client

- Event driven
Duration = 0.131s for 100 clients, 100 reqs per client
Duration = 1.067s for 1000 clients, 100 reqs per client

Below are values for Event-driven server with parsing and file reading -

- Without caching
Duration = 0.193s for 100 clients, 100 reqs per client
Duration = 2.253s for 1000 clients, 100 reqs per client

- With per-client caching of file
Duration = 0.118s for 100 clients, 100 reqs per client
Duration = 1.215s for 1000 clients, 100 reqs per client
