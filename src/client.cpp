#include "clientservice.h"
#include "ioservice.h"
#include "responsecontext.h"
#include <asyncgi/client.h>
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/io.h>
#include <asyncgi/response.h>

namespace asyncgi {

Client::Client(IO& io)
    : clientService_{std::make_unique<detail::ClientService>(
              io.ioService(sfun::access_token<Client>{}).io(),
              io.errorHandler(sfun::access_token<Client>{}))}
{
}

Client::Client(Response& response)
    : clientService_{response.context(sfun::access_token<Client>{}).client()}
{
}

void Client::makeRequest(
        const std::filesystem::path& socketPath,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_.get().makeRequest(socketPath, std::move(request), std::move(responseHandler), timeout);
}

void Client::makeRequest(
        const std::filesystem::path& socketPath,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_.get().makeRequest(socketPath, request, responseHandler, timeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_.get().makeRequest(ipAddress, port, std::move(request), std::move(responseHandler), timeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    clientService_.get().makeRequest(ipAddress, port, request, responseHandler, timeout);
}

void Client::disconnect()
{
    clientService_.get().disconnect();
}

} //namespace asyncgi
