#pragma once

namespace asio{
    class io_context;
}

namespace asyncgi::detail{
class IRuntime{
public:
    IRuntime() = default;
    virtual ~IRuntime() = default;
    IRuntime(const IRuntime&) = delete;
    IRuntime& operator=(const IRuntime&) = delete;
    IRuntime(IRuntime&&) = delete;
    IRuntime&& operator=(IRuntime&&) = delete;

    virtual asio::io_context& io() = 0;
    virtual asio::io_context& nextIO() = 0;
    virtual void run() = 0;
    virtual void stop() = 0;

};
}
