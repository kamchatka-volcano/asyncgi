#include "clientservice.h"
#include "ioservice.h"
#include "timerprovider.h"
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/write.hpp>
#else
#include <asio/write.hpp>
#endif
#include <asyncgi/detail/external/sfun/utility.h>
#include <asyncgi/detail/external/whaleroute/requestprocessorqueue.h>

namespace asyncgi::detail {

ClientService::ClientService(asio::io_context& io, EventHandlerProxy& eventHandler)
    : io_{io}
    , eventHandler_{eventHandler}
    , timerProvider_{io_}
{
}

void ClientService::makeRequest(
        const fs::path& socketPath,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([] {});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(
            timeout,
            [=]
            {
                (*cancelRequestOnTimeout)();
            });

    auto onResponseReceived = [=, &responseTimeoutTimer](std::optional<fcgi::ResponseData> fcgiResponse)
    {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
            if (!fcgiResponse->errorMsg.empty())
                eventHandler_(ErrorEvent::RequestProcessingError, fcgiResponse->errorMsg);
            responseHandler(fastcgi::Response{std::move(fcgiResponse->data), std::move(fcgiResponse->errorMsg)});
        }
        else
            responseHandler(std::nullopt);

        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection = localClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, eventHandler_));
    clientConnection->makeRequest(
            asio::local::stream_protocol::endpoint{socketPath.string()},
            std::move(request),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void ClientService::makeRequest(
        const fs::path& socketPath,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();
    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([] {});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(
            timeout,
            [=]
            {
                (*cancelRequestOnTimeout)();
            });
    auto onResponseReceived = [=, &responseTimeoutTimer](std::optional<fcgi::ResponseData> fcgiResponse)
    {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
            if (!fcgiResponse->errorMsg.empty())
                eventHandler_(ErrorEvent::RequestProcessingError, fcgiResponse->errorMsg);
            responseHandler(http::responseFromString(fcgiResponse->data));
        }
        else
            responseHandler(std::nullopt);
        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection = localClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, eventHandler_));
    auto [params, stdIn] = request.toFcgiData(http::FormType::Multipart);
    auto fcgiRequest = fastcgi::Request{std::move(params), std::move(stdIn)};
    clientConnection->makeRequest(
            asio::local::stream_protocol::endpoint{socketPath.string()},
            std::move(fcgiRequest),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void ClientService::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        fastcgi::Request request,
        std::function<void(std::optional<fastcgi::Response>)> responseHandler,
        std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();
    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([] {});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(
            timeout,
            [=]
            {
                (*cancelRequestOnTimeout)();
            });
    auto onResponseReceived = [=, &responseTimeoutTimer](std::optional<fcgi::ResponseData> fcgiResponse)
    {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
            if (!fcgiResponse->errorMsg.empty())
                eventHandler_(ErrorEvent::RequestProcessingError, fcgiResponse->errorMsg);
            responseHandler(fastcgi::Response{std::move(fcgiResponse->data), std::move(fcgiResponse->errorMsg)});
        }
        else
            responseHandler(std::nullopt);
        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection =
            tcpClientConnections_.emplace_back(std::make_unique<ClientConnection<asio::ip::tcp>>(io_, eventHandler_));
    auto address = asio::ip::make_address(ipAddress.data());
    clientConnection->makeRequest(
            asio::ip::tcp::endpoint{address, port},
            std::move(request),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void ClientService::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();
    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([] {});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(
            timeout,
            [=]
            {
                (*cancelRequestOnTimeout)();
            });

    auto onResponseReceived = [=, &responseTimeoutTimer](std::optional<fcgi::ResponseData> fcgiResponse)
    {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
            if (!fcgiResponse->errorMsg.empty())
                eventHandler_(ErrorEvent::RequestProcessingError, fcgiResponse->errorMsg);
            responseHandler(http::responseFromString(fcgiResponse->data));
        }
        else
            responseHandler(std::nullopt);
        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection =
            tcpClientConnections_.emplace_back(std::make_unique<ClientConnection<asio::ip::tcp>>(io_, eventHandler_));
    auto [params, stdIn] = request.toFcgiData(http::FormType::Multipart);
    auto fcgiRequest = fastcgi::Request{std::move(params), std::move(stdIn)};
    auto address = asio::ip::make_address(ipAddress);
    clientConnection->makeRequest(
            asio::ip::tcp::endpoint{address, port},
            std::move(fcgiRequest),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void ClientService::disconnect()
{
    localClientConnections_.clear();
}

void ClientService::setRequestProcessorQueue(whaleroute::RequestProcessorQueue& queue)
{
    requestProcessorQueue_.get().emplace(queue);
}

} // namespace asyncgi::detail