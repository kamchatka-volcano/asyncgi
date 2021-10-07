#include <asyncgi/server.h>
#include <asyncgi/iruntime.h>
#include "connectionprocessor.h"
#include "connectionfactory.h"
#include "alias_unixdomain.h"
#include <asio/error_code.hpp>
#include <asio/error.hpp>
#include <iostream>

namespace asyncgi{

namespace fs = std::filesystem;

Server::Server(asio::io_context& io, std::unique_ptr<ConnectionFactory> connectionFactory, ErrorHandlerFunc errorHandler)
    : io_(io)
    , connectionFactory_(std::move(connectionFactory))
    , errorHandler_(errorHandler)
{    
}

Server::~Server() = default;

void Server::listen(const fs::path& socketPath)
{
    initUnixDomainSocket(socketPath);
    auto connectionListener = std::make_unique<unixdomain::acceptor>(io_, unixdomain::endpoint{socketPath});
    connectionProcessor_ = std::make_unique<ConnectionProcessor>(std::move(connectionListener), *connectionFactory_, errorHandler_);
}

void Server::initUnixDomainSocket(const fs::path& path)
{
    umask(0);
    chmod(path.c_str(), 0777);
    unlink(path.c_str());
}

}
