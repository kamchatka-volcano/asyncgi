#pragma once

namespace asio{
    class io_context;
}

namespace asyncgi::detail{
class IRuntime{
public:
    virtual ~IRuntime() = default;
    virtual asio::io_context& io() = 0;
    virtual asio::io_context& nextIO() = 0;
    virtual void run() = 0;
    virtual void stop() = 0;

};
}
