#pragma once
#include "connectionlistener.h"
#include <asyncgi/errors.h>
#include <memory>


namespace asio{
    class io_context;
}

namespace asyncgi::detail{
class ConnectionFactory;

class ConnectionListenerFactory {
public:
    ConnectionListenerFactory(asio::io_context&, std::unique_ptr<ConnectionFactory>, ErrorHandlerFunc);
    ~ConnectionListenerFactory();

    template<typename TProtocol>
    std::unique_ptr<ConnectionListener<TProtocol>> makeConnectionListener(const typename TProtocol::endpoint& address);

private:
    asio::io_context& io_;
    std::unique_ptr<ConnectionFactory> connectionFactory_;
    ErrorHandler errorHandler_;
};

}
