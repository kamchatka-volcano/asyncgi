#include "serverservice.h"
#include "connectionlistener.h"
#include "connectionlistenerfactory.h"
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/external/sfun/contract.h>
#include <asyncgi/errors.h>
#include <cstdint>

namespace asyncgi::detail {

namespace fs = std::filesystem;

ServerService::ServerService(std::unique_ptr<ConnectionListenerFactory> connectionListenerFactory)
    : connectionListenerFactory_{std::move(connectionListenerFactory)}
{
}

ServerService::~ServerService() = default;

#ifndef _WIN32
namespace {
void initUnixDomainSocket(const fs::path& path)
{
    umask(0);
    chmod(path.c_str(), 0777);
    unlink(path.c_str());
}
} // namespace
#endif

void ServerService::listen(const fs::path& socketPath)
{
#ifndef _WIN32
    initUnixDomainSocket(socketPath);
    localConnectionProcessors_.emplace_back(
            connectionListenerFactory_->makeConnectionListener<asio::local::stream_protocol>(
                    asio::local::stream_protocol::endpoint{socketPath}));
#else
    [[maybe_unused]] auto& unused = socketPath;
    throw Error{"Unix domain hosts aren't supported on Windows. Use a TCP host with Server::listen(std::string_view "
                "ipAddress, uint16_t portNumber)."};
#endif
}

void ServerService::listen(std::string_view ipAddress, int portNumber)
{
    sfun_precondition(portNumber >= 0 && portNumber < 65536);

    auto address = asio::ip::make_address(ipAddress.data());
    tcpConnectionProcessors_.emplace_back(connectionListenerFactory_->makeConnectionListener<asio::ip::tcp>(
            asio::ip::tcp::endpoint{address, static_cast<std::uint16_t>(portNumber)}));
}

} // namespace asyncgi::detail
