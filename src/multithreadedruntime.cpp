#include "multithreadedruntime.h"

namespace asyncgi{

MultithreadedRuntime::MultithreadedRuntime(std::size_t threadCount)
    : ioGuard_(io_.get_executor())
{
    for(auto i = 0u; i < threadCount; ++i){
        auto io = std::make_shared<asio::io_context>();
        ioGuardPool_.emplace_back(std::make_shared<asio_work_guard>(io->get_executor()));
        ioPool_.emplace_back(std::move(io));
    }
}

void MultithreadedRuntime::run()
{
    for(auto& io_context : ioPool_)
        threadPool_.emplace_back([&]
        {
            io_context->run();
        });
    io_.run();
    for(auto& thread : threadPool_)
        thread.join();
}

void MultithreadedRuntime::stop()
{
    ioGuard_.reset();
    ioGuardPool_.clear();
    for (auto& io : ioPool_)
        io->stop();
    io_.stop();
}

asio::io_context& MultithreadedRuntime::nextIO()
{
    return *ioPool_[ioIndex_++ % ioPool_.size()];
}

}
