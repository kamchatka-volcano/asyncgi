#include "clientservice.h"
#include "ioservice.h"
#include "timerprovider.h"
#include <asio/write.hpp>
#include <asyncgi/detail/external/whaleroute/requestprocessorqueue.h>

namespace asyncgi::detail {

ClientService::ClientService(asio::io_context& io, ErrorHandler& errorHandler)
    : io_{io}
    , errorHandler_{errorHandler}
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
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(fastcgi::Response{std::move(fcgiResponse->data), std::move(fcgiResponse->errorMsg)});
        }
        else
            responseHandler(std::nullopt);

        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection = localClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, errorHandler_));
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
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(http::responseFromString(fcgiResponse->data));
        }
        else
            responseHandler(std::nullopt);
        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection = localClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, errorHandler_));
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
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(fastcgi::Response{std::move(fcgiResponse->data), std::move(fcgiResponse->errorMsg)});
        }
        else
            responseHandler(std::nullopt);
        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection =
            tcpClientConnections_.emplace_back(std::make_unique<ClientConnection<asio::ip::tcp>>(io_, errorHandler_));
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
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(http::responseFromString(fcgiResponse->data));
        }
        else
            responseHandler(std::nullopt);
        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto& clientConnection =
            tcpClientConnections_.emplace_back(std::make_unique<ClientConnection<asio::ip::tcp>>(io_, errorHandler_));
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

void ClientService::setRequestProcessorQueue(whaleroute::RequestProcessorQueue* queue)
{
    requestProcessorQueue_ = queue;
}

} // namespace asyncgi::detail