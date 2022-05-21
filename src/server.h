#pragma once
#include "alias_unixdomain.h"
#include <asyncgi/iserver.h>
#include <asyncgi/types.h>
#include <asyncgi/errors.h>
#include <filesystem>
#include <string>
#include <memory>

namespace asio{
class io_context;
}

namespace asyncgi::detail{
class ConnectionFactory;
class Connection;

class Server : public IServer{
public:
    Server(asio::io_context&, std::unique_ptr<detail::ConnectionFactory>, ErrorHandlerFunc);
    ~Server() override;
    void listen(const std::filesystem::path& socketPath) override;

private:
    void waitForConnection();
    void onConnected(Connection& connection, const std::error_code& error);

private:    
    asio::io_context& io_;
    std::unique_ptr<detail::ConnectionFactory> connectionFactory_;
    ErrorHandler errorHandler_;
    std::unique_ptr<unixdomain::acceptor> connectionListener_;
};

}
