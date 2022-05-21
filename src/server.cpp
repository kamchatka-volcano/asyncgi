#include "server.h"
#include "connection.h"
#include "connectionfactory.h"
#include "iruntime.h"
#include "alias_unixdomain.h"
#include <asio/error_code.hpp>

namespace asyncgi::detail{

namespace fs = std::filesystem;

Server::Server(asio::io_context& io, std::unique_ptr<ConnectionFactory> connectionFactory, ErrorHandlerFunc errorHandler)
    : io_(io)
    , connectionFactory_(std::move(connectionFactory))
    , errorHandler_(std::move(errorHandler))
{    
}

Server::~Server() = default;

namespace {
void initUnixDomainSocket(const fs::path& path)
{
    umask(0);
    chmod(path.c_str(), 0777);
    unlink(path.c_str());
}
}

void Server::listen(const fs::path& socketPath)
{
    initUnixDomainSocket(socketPath);
    connectionListener_ = std::make_unique<unixdomain::acceptor>(io_, unixdomain::endpoint{socketPath});
    waitForConnection();
}

void Server::waitForConnection()
{
    auto connection = connectionFactory_->makeConnection();
    connectionListener_->async_accept(connection->socket(),
        [this, connection](auto error_code){
            onConnected(*connection, error_code);
        });
}

void Server::onConnected(Connection& connection, const std::error_code& error)
{
    if (error){
        errorHandler_(ErrorType::ConnectionError, error);
        return;
    }
    connection.process();
    waitForConnection();
}

}
