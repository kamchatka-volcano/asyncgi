#include "connectionlistenerfactory.h"
#include "connectionfactory.h"
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#else
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#endif

namespace asyncgi::detail {

ConnectionListenerFactory::ConnectionListenerFactory(
        asio::io_context& io,
        std::unique_ptr<ConnectionFactory> connectionFactory,
        EventHandlerProxy& eventHandler)
    : io_{io}
    , connectionFactory_{std::move(connectionFactory)}
    , eventHandler_{eventHandler}
{
}

ConnectionListenerFactory::~ConnectionListenerFactory() = default;

template<typename TProtocol>
std::unique_ptr<ConnectionListener<TProtocol>> ConnectionListenerFactory::makeConnectionListener(
        const typename TProtocol::endpoint& address)
{
    return std::make_unique<ConnectionListener<TProtocol>>(
            std::make_unique<typename TProtocol::acceptor>(io_.get(), address),
            *connectionFactory_,
            eventHandler_);
}

template std::unique_ptr<ConnectionListener<asio::local::stream_protocol>> ConnectionListenerFactory::
        makeConnectionListener(const asio::local::stream_protocol::endpoint& address);
template std::unique_ptr<ConnectionListener<asio::ip::tcp>> ConnectionListenerFactory::makeConnectionListener(
        const asio::ip::tcp::endpoint& address);
} // namespace asyncgi::detail