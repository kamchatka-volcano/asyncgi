#include "connectionfactory.h"
#include "connectionlistenerfactory.h"
#include "serverservice.h"
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/io.h>
#include <asyncgi/requestprocessor.h>
#include <asyncgi/server.h>

namespace asyncgi {

namespace detail {
auto makeConnectionListenerFactory(
        RequestProcessor requestProcessor,
        IO& io,
        const sfun::access_token<Server>& accessToken)
{
    auto connectionFactory = std::make_unique<detail::ConnectionFactory>(
            std::move(requestProcessor),
            io.ioService(accessToken),
            io.errorHandler());

    return std::make_unique<detail::ConnectionListenerFactory>(
            io.ioService(accessToken).io(),
            std::move(connectionFactory),
            io.errorHandler());
}
} //namespace detail

Server::Server(IO& io, RequestProcessor requestProcessor)
    : serverService_{std::make_unique<detail::ServerService>(
              detail::makeConnectionListenerFactory(std::move(requestProcessor), io, sfun::access_token<Server>{}))}
{
}

Server::~Server() = default;

void Server::listen(std::string_view ipAddress, int port)
{
    if (port < 0 || port > 65535)
        throw Error{"port's value must be in range [0, 65535]"};

    serverService_->listen(ipAddress, port);
}

void Server::listen(const std::filesystem::path& unixDomainSocket)
{
    serverService_->listen(unixDomainSocket);
}

} //namespace asyncgi