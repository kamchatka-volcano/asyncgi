#pragma once
#include <asyncgi/detail/external/sfun/interface.h>

namespace asio {
class io_context;
}

namespace asyncgi::detail {
class IRuntime : private sfun::Interface<IRuntime> {
public:
    virtual asio::io_context& io() = 0;
    virtual asio::io_context& nextIO() = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
};
} // namespace asyncgi::detail
