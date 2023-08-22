#include "clientservice.h"
#include "ioservice.h"
#include "responsecontext.h"
#include <asyncgi/client.h>
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/detail/external/sfun/optional_ref.h>
#include <asyncgi/io.h>
#include <asyncgi/response.h>

namespace asyncgi {

Client::Client(IO& io)
    : clientService_{std::make_unique<detail::ClientService>(
              io.ioService(sfun::access_token<Client>{}).io(),
              io.eventHandler(sfun::access_token<Client>{}))}
{
}

namespace {
sfun::optional_ref<detail::ClientService> getClientService(Response& response, sfun::access_token<Client> accessToken)
{
    if (auto context = response.context(accessToken).lock())
        return context->client();
    else
        return std::nullopt;
}
} //namespace

Client::Client(Response& response)
    : clientService_{getClientService(response, sfun::access_token<Client>{})}
{
}

void Client::makeRequest(
        const std::filesystem::path& socketPath,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!clientService_.has_value())
        return;

    clientService_.get().makeRequest(socketPath, std::move(request), std::move(responseHandler), timeout);
}

void Client::makeRequest(
        const std::filesystem::path& socketPath,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!clientService_.has_value())
        return;

    clientService_.get().makeRequest(socketPath, request, responseHandler, timeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!clientService_.has_value())
        return;

    clientService_.get().makeRequest(ipAddress, port, std::move(request), std::move(responseHandler), timeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!clientService_.has_value())
        return;

    clientService_.get().makeRequest(ipAddress, port, request, responseHandler, timeout);
}

void Client::disconnect()
{
    if (!clientService_.has_value())
        return;

    clientService_.get().disconnect();
}

} //namespace asyncgi
