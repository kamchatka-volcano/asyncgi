#include "connectionlistener.h"
#include "connectionfactory.h"
#include "connection.h"
#include "iruntime.h"
#include <asio/local/stream_protocol.hpp>
#include <asio/ip/tcp.hpp>

namespace asyncgi::detail{

template<typename TProtocol>
ConnectionListener<TProtocol>::ConnectionListener(std::unique_ptr<asio::basic_socket_acceptor<TProtocol>> socketAcceptor,
                                                  ConnectionFactory& connectionFactory,
                                                  ErrorHandlerFunc errorHandler)
    : socketAcceptor_{std::move(socketAcceptor)}
    , connectionFactory_{connectionFactory}
    , errorHandler_{std::move(errorHandler)}

{
    waitForConnection();
}

template<typename TProtocol>
void ConnectionListener<TProtocol>::waitForConnection()
{
    auto connection = connectionFactory_.makeConnection<TProtocol>();
    socketAcceptor_->async_accept(connection->socket(),
                                  [this, connection](auto error_code) {
                                      onConnected(*connection, error_code);
                                  });
}

template<typename TProtocol>
void ConnectionListener<TProtocol>::onConnected(Connection<TProtocol>& connection, const std::error_code& error)
{
    if (error){
        errorHandler_(ErrorType::ConnectionError, error);
        return;
    }
    connection.process();
    waitForConnection();
}

template class ConnectionListener<asio::local::stream_protocol>;
template class ConnectionListener<asio::ip::tcp>;

}
