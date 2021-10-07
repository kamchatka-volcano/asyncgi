#pragma once
#include "runtime.h"
#include <asio/io_context.hpp>
#include <vector>
#include <memory>
#include <thread>

namespace asyncgi{
using asio_work_guard = asio::executor_work_guard<asio::io_context::executor_type>;

class MultithreadedRuntime : public Runtime
{
public:
    MultithreadedRuntime(std::size_t threadCount);    
    asio::io_context& nextIO() final;
    void run() final;
    void stop() final;

private:
    asio_work_guard ioGuard_;
    std::vector<std::shared_ptr<asio::io_context>> ioPool_;
    std::vector<std::shared_ptr<asio_work_guard>> ioGuardPool_;
    std::vector<std::thread> threadPool_;
    std::atomic<std::size_t> ioIndex_ = 0;
};

}
