#pragma once
#include <asyncgi/iasiodispatcher.h>

namespace asio{
    class io_context;
}

namespace asyncgi::detail{

class AsioDispatcher : public IAsioDispatcher{
public:
    AsioDispatcher(asio::io_context& io);
    void dispatch(std::function<void(asio::io_context&)>) override;

private:
    asio::io_context& io_;
};

}
