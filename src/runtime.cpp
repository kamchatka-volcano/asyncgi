#include "runtime.h"
#include <asio/signal_set.hpp>

namespace asyncgi::detail{

Runtime::Runtime()
    : signals_{io_, SIGINT, SIGTERM}
{
    handleStopSignals();
}

asio::io_context& Runtime::io()
{
    return io_;
}

asio::io_context& Runtime::nextIO()
{
    return io_;
}

void Runtime::run()
{
    io_.run();
}

void Runtime::stop()
{
    io_.stop();
}

void Runtime::handleStopSignals()
{
    signals_.async_wait([&](auto error, auto){
        if (!error){
            stop();
            return;
        }
        handleStopSignals();
    });
}

asio::io_context& Runtime::defaultIO()
{
    return io_;
}

}
