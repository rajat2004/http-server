#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <cassert>

class ThreadPool {
public:
    ThreadPool(size_t n_threads)
    : stop(false)
    {
        assert(n_threads>0);

        for (size_t i=0; i<n_threads; ++i) {
            workers.emplace_back([this] {
                // Infinite loop to pick tasks and do it
                while (true) {
                    std::packaged_task<void()> task;

                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->cv.wait(lock, [this] {
                        return this->stop || !this->job_queue.empty();
                    });

                    if (this->stop || this->job_queue.empty())
                        return;

                    task = std::move(this->job_queue.front());
                    this->job_queue.pop();

                    task();
                }

            });
        }
    }

    // Join all threads
    ~ThreadPool()
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;

        cv.notify_all();
        for(auto &worker : workers)
            worker.join();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool &operator=(const ThreadPool&) = delete;


    // add new work item to the pool
    template<class F, class... Args>
    decltype(auto) enqueue(F&& f, Args&&... args)
    {
        using return_type = decltype(f(args...));

        std::packaged_task<return_type()> task(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task.get_future();

        std::unique_lock<std::mutex> lock(queue_mutex);

        if (stop)
            throw std::runtime_error("Enqueue after stopping ThreadPool");

        job_queue.emplace(std::move(task));

        cv.notify_one();
        return res;
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::packaged_task<void()>> job_queue;

    std::mutex queue_mutex;
    std::condition_variable cv;
    bool stop = false;
};


#endif // THREAD_POOL_H
