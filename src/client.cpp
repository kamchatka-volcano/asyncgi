#include "clientservice.h"
#include "ioservice.h"
#include <asyncgi/client.h>
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/io.h>

namespace asyncgi {

Client::Client(IO& io)
    : clientService_{std::make_unique<detail::ClientService>(
              io.ioService(sfun::access_token<Client>{}).io(),
              io.errorHandler())}
{
}

Client::~Client() = default;

void Client::makeRequest(
        const std::filesystem::path& socketPath,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_->makeRequest(socketPath, std::move(request), std::move(responseHandler), timeout);
}

void Client::makeRequest(
        const std::filesystem::path& socketPath,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_->makeRequest(socketPath, request, responseHandler, timeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_->makeRequest(ipAddress, port, std::move(request), std::move(responseHandler), timeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_->makeRequest(ipAddress, port, request, responseHandler, timeout);
}

void Client::disconnect()
{
    clientService_->disconnect();
}

} //namespace asyncgi
