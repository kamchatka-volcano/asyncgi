#pragma once
#include <asio/basic_socket_acceptor.hpp>
#include <asyncgi/errors.h>
#include <string>

namespace asyncgi::detail {
template<typename TProtocol>
class Connection;
class ConnectionFactory;

template<typename TProtocol>
class ConnectionListener {
public:
    ConnectionListener(
            std::unique_ptr<asio::basic_socket_acceptor<TProtocol>> socketAcceptor,
            ConnectionFactory& connectionFactory,
            ErrorHandlerFunc errorHandler);

private:
    void waitForConnection();
    void onConnected(Connection<TProtocol>& connection, const std::error_code& error);

private:
    std::unique_ptr<asio::basic_socket_acceptor<TProtocol>> socketAcceptor_;
    ConnectionFactory& connectionFactory_;
    ErrorHandler errorHandler_;
};

} // namespace asyncgi::detail
