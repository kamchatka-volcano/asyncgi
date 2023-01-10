#include "connectionlistenerfactory.h"
#include "connectionfactory.h"
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>

namespace asyncgi::detail {

ConnectionListenerFactory::ConnectionListenerFactory(
        asio::io_context& io,
        std::unique_ptr<ConnectionFactory> connectionFactory,
        ErrorHandlerFunc errorHandler)
    : io_{io}
    , connectionFactory_{std::move(connectionFactory)}
    , errorHandler_{std::move(errorHandler)}
{
}

ConnectionListenerFactory::~ConnectionListenerFactory() = default;

template<typename TProtocol>
std::unique_ptr<ConnectionListener<TProtocol>> ConnectionListenerFactory::makeConnectionListener(
        const typename TProtocol::endpoint& address)
{
    return std::make_unique<ConnectionListener<TProtocol>>(
            std::make_unique<typename TProtocol::acceptor>(io_, address),
            *connectionFactory_,
            errorHandler_);
}

template std::unique_ptr<ConnectionListener<asio::local::stream_protocol>> ConnectionListenerFactory::
        makeConnectionListener(const asio::local::stream_protocol::endpoint& address);
template std::unique_ptr<ConnectionListener<asio::ip::tcp>> ConnectionListenerFactory::makeConnectionListener(
        const asio::ip::tcp::endpoint& address);
} // namespace asyncgi::detail