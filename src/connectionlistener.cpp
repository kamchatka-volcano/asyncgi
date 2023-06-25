#include "connectionlistener.h"
#include "connection.h"
#include "connectionfactory.h"
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>

namespace asyncgi::detail {

template<typename TProtocol>
ConnectionListener<TProtocol>::ConnectionListener(
        std::unique_ptr<asio::basic_socket_acceptor<TProtocol>> socketAcceptor,
        ConnectionFactory& connectionFactory,
        ErrorHandler& errorHandler)
    : socketAcceptor_{std::move(socketAcceptor)}
    , connectionFactory_{connectionFactory}
    , errorHandler_{errorHandler}

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
        errorHandler_(ErrorType::ConnectionError, error);
        return;
    }
    connection.process();
    waitForConnection();
}

template class ConnectionListener<asio::local::stream_protocol>;
template class ConnectionListener<asio::ip::tcp>;

} // namespace asyncgi::detail
