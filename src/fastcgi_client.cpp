#include "clientservice.h"
#include "ioservice.h"
#include "responsecontext.h"
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/detail/external/sfun/optional_ref.h>
#include <asyncgi/fastcgi_client.h>
#include <asyncgi/io.h>
#include <asyncgi/responder.h>

namespace asyncgi {

FastCgiClient::FastCgiClient(IO& io)
    : clientService_{std::make_unique<detail::ClientService>(
              io.ioService(sfun::access_token<FastCgiClient>{}).io(),
              io.eventHandler(sfun::access_token<FastCgiClient>{}))}
{
}

namespace {
sfun::optional_ref<detail::ClientService> getClientService(
        Responder& response,
        sfun::access_token<FastCgiClient> accessToken)
{
    if (auto context = response.context(accessToken).lock())
        return context->client();
    else
        return std::nullopt;
}
} //namespace

FastCgiClient::FastCgiClient(Responder& response)
    : clientService_{getClientService(response, sfun::access_token<FastCgiClient>{})}
{
}

void FastCgiClient::makeRequest(
        const std::filesystem::path& socketPath,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!clientService_.has_value())
        return;

    clientService_.get().makeRequest(socketPath, std::move(request), std::move(responseHandler), timeout);
}

void FastCgiClient::makeRequest(
        const std::filesystem::path& socketPath,
        const http::Request& request,
        const std::function<void(std::optional<http::Response>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!clientService_.has_value())
        return;

    clientService_.get().makeRequest(socketPath, request, responseHandler, timeout);
}

void FastCgiClient::makeRequest(
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

void FastCgiClient::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(std::optional<http::Response>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!clientService_.has_value())
        return;

    clientService_.get().makeRequest(ipAddress, port, request, responseHandler, timeout);
}

void FastCgiClient::disconnect()
{
    if (!clientService_.has_value())
        return;

    clientService_.get().disconnect();
}

} //namespace asyncgi
