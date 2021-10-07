#pragma once
#include <asyncgi/iruntime.h>
#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>

namespace asyncgi{
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
    asio::io_context io_;

private:
    asio::signal_set signals_;
};

}
