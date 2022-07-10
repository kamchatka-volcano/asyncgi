#include "server.h"
#include "connectionlistenerfactory.h"
#include "connectionlistener.h"
#include <asio/error_code.hpp>

namespace asyncgi::detail{

namespace fs = std::filesystem;

Server::Server(std::unique_ptr<ConnectionListenerFactory> connectionListenerFactory)
    : connectionListenerFactory_{std::move(connectionListenerFactory)}
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
    localConnectionProcessors_.emplace_back(
            connectionListenerFactory_->makeConnectionListener<asio::local::stream_protocol>(
                    asio::local::stream_protocol::endpoint{socketPath}));
}

void Server::listen(std::string_view ipAddress, uint16_t portNumber)
{
    auto address = asio::ip::make_address(ipAddress);
    tcpConnectionProcessors_.emplace_back(connectionListenerFactory_->makeConnectionListener<asio::ip::tcp>(
            asio::ip::tcp::endpoint{address, portNumber}));
}

}
