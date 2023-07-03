#pragma once
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/basic_socket_acceptor.hpp>
#else
#include <asio/basic_socket_acceptor.hpp>
#endif
#include <asyncgi/detail/external/sfun/member.h>
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
            ErrorHandler& errorHandler);

private:
    void waitForConnection();
    void onConnected(Connection<TProtocol>& connection, const std::error_code& error);

private:
    std::unique_ptr<asio::basic_socket_acceptor<TProtocol>> socketAcceptor_;
    sfun::member<ConnectionFactory&> connectionFactory_;
    sfun::member<ErrorHandler&> errorHandler_;
};

} // namespace asyncgi::detail
