#pragma once
#include<functional>

namespace asio{
    class io_context;
}

namespace asyncgi{

class IAsioDispatcher{
public:
    IAsioDispatcher() = default;
    virtual ~IAsioDispatcher() = default;
    IAsioDispatcher(const IAsioDispatcher&) = delete;
    IAsioDispatcher& operator=(const IAsioDispatcher&) = delete;
    IAsioDispatcher(IAsioDispatcher&&) = delete;
    IAsioDispatcher&& operator=(IAsioDispatcher&&) = delete;

    virtual void dispatch(std::function<void(asio::io_context&)>) = 0;
};

}