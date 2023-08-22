#include "connectionlistener.h"
#include "connection.h"
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

template<typename TProtocol>
ConnectionListener<TProtocol>::ConnectionListener(
        std::unique_ptr<asio::basic_socket_acceptor<TProtocol>> socketAcceptor,
        ConnectionFactory& connectionFactory,
        EventHandlerProxy& eventHandler)
    : socketAcceptor_{std::move(socketAcceptor)}
    , connectionFactory_{connectionFactory}
    , eventHandler_{eventHandler}

{
    waitForConnection();
}

template<typename TProtocol>
void ConnectionListener<TProtocol>::waitForConnection()
{
    auto connection = connectionFactory_.get().makeConnection<TProtocol>();
    socketAcceptor_->async_accept(
            connection->socket(),
            [this, connection](auto error_code)
            {
                onConnected(*connection, error_code);
            });
}

template<typename TProtocol>
void ConnectionListener<TProtocol>::onConnected(Connection<TProtocol>& connection, const std::error_code& error)
{
    if (error) {
        eventHandler_(ErrorEvent::ConnectionError, error.message());
        return;
    }
    connection.process();
    waitForConnection();
}

template class ConnectionListener<asio::local::stream_protocol>;
template class ConnectionListener<asio::ip::tcp>;

} // namespace asyncgi::detail
