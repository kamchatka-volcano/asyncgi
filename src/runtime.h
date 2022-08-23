#pragma once
#include "iruntime.h"
#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>

namespace asyncgi::detail{
class Runtime : public IRuntime{
public:
    Runtime();
    asio::io_context& io() override;
    asio::io_context& nextIO() override;
    void run() override;
    void stop() override;

private:
    void handleStopSignals();

protected:
    asio::io_context& defaultIO();

private:
    asio::io_context io_;
    asio::signal_set signals_;
};

}
