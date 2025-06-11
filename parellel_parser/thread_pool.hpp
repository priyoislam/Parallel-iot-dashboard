#pragma once
#include <vector>
#include <thread>
#include <functional>

class ThreadPool {
private:
    std::vector<std::thread> workers_;

public:
    template<typename F>
    void start(int num_threads, F worker_function) {
        for (int i = 0; i < num_threads; ++i) {
            workers_.emplace_back(worker_function);
        }
    }

    void join() {
        for (auto& worker : workers_) {
            if (worker.joinable()) worker.join();
        }
    }
};