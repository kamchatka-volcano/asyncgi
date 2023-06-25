#include "ioservice.h"
#include <asio/signal_set.hpp>
#include <sfun/contract.h>
#include <sfun/utility.h>

#include <iostream>

namespace asyncgi::detail {

namespace {
std::vector<std::unique_ptr<asio::io_context>> makeIoPool(int threadCount)
{
    auto res = std::vector<std::unique_ptr<asio::io_context>>{};
    for (auto i = 0; i < threadCount; ++i)
        res.emplace_back(std::make_unique<asio::io_context>());
    return res;
}
} //namespace

IOService::IOService(int threadCount)
    : ioPool_{makeIoPool(threadCount)}
    , signals_{
              *ioPool_.at(0),
              SIGINT,
#ifndef _WIN32
              SIGQUIT,
#endif
              SIGTERM}
{
    handleStopSignals();
}

asio::io_context& IOService::io() const
{
    return *ioPool_.at(0);
}

asio::io_context& IOService::nextIO()
{
    return *ioPool_[ioIndex_++ % ioPool_.size()];
}

void IOService::run()
{
    for (auto& io : ioPool_)
        ioGuardPool_.emplace_back(std::make_unique<asio_work_guard>(io->get_executor()));

    for (auto i = 1; i < sfun::ssize(ioPool_); ++i) {
        threadPool_.emplace_back(
                [this, i]
                {
                    ioPool_.at(i)->run();
                });
    }

    ioPool_.at(0)->run();
    for (auto& thread : threadPool_)
        thread.join();
}

void IOService::stop()
{
    ioGuardPool_.clear();
    for (auto& io : ioPool_)
        io->stop();
}

void IOService::handleStopSignals()
{
    signals_.async_wait(
            [&](auto error, auto)
            {
                if (!error) {
                    stop();
                    return;
                }
                handleStopSignals();
            });
}

} // namespace asyncgi::detail
