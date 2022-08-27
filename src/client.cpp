#include "client.h"
#include "timerprovider.h"
#include <asio/write.hpp>

namespace asyncgi::detail{

Client::Client(asio::io_context& io, ErrorHandlerFunc errorHandler)
    : io_{io}
    , timerProvider_{io}
    , errorHandler_{std::move(errorHandler)}
{}

void Client::makeRequest(
        const fs::path& socketPath,
        fastcgi::Request request,
        std::function<void(const std::optional<fastcgi::Response>&)> responseHandler)
{
    makeRequest(
            socketPath,
            std::move(request),
            std::move(responseHandler),
            std::chrono::seconds{3});
}

void Client::makeRequest(
        const fs::path& socketPath,
        fastcgi::Request request,
        std::function<void(const std::optional<fastcgi::Response>&)> responseHandler,
        const std::chrono::milliseconds& timeout)
{
    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([]{});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(timeout, [=] {
        (*cancelRequestOnTimeout)();
    }, TimerMode::Once);

    auto onResponseReceived = [=, &responseTimeoutTimer](const std::optional<fcgi::ResponseData>& fcgiResponse) {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
            if (!fcgiResponse->errorMsg.empty())
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(fastcgi::Response{fcgiResponse->data, fcgiResponse->errorMsg});
        }
        else
            responseHandler(std::nullopt);
    };
    auto& clientConnection = localClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, errorHandler_));
    clientConnection->makeRequest(
            asio::local::stream_protocol::endpoint{socketPath},
            std::move(request),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void Client::makeRequest(
        const fs::path& socketPath,
        const http::Request& request,
        const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler)
{
    makeRequest(
            socketPath,
            request,
            responseHandler,
            std::chrono::seconds{3});
}

void Client::makeRequest(
        const fs::path& socketPath,
        const http::Request& request,
        const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler,
        const std::chrono::milliseconds& timeout)
{
    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([]{});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(timeout, [=] {
        (*cancelRequestOnTimeout)();
    }, TimerMode::Once);
    auto onResponseReceived = [=, &responseTimeoutTimer](const std::optional<fcgi::ResponseData>& fcgiResponse) {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
             if (!fcgiResponse->errorMsg.empty())
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(http::responseFromString(fcgiResponse->data));
        }
        else
            responseHandler(std::nullopt);
    };
    auto& clientConnection = localClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, errorHandler_));
    auto [params, stdIn] = request.toFcgiData(http::FormType::Multipart);
    auto fcgiRequest = fastcgi::Request{std::move(params), std::move(stdIn)};
    clientConnection->makeRequest(
            asio::local::stream_protocol::endpoint{socketPath},
            std::move(fcgiRequest),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        fastcgi::Request request,
        std::function<void(const std::optional<fastcgi::Response>&)> responseHandler)
{
    makeRequest(
            ipAddress,
            port,
            std::move(request),
            std::move(responseHandler),
            std::chrono::seconds{3});
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        fastcgi::Request request,
        std::function<void(const std::optional<fastcgi::Response>&)> responseHandler,
        const std::chrono::milliseconds& timeout)
{
    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([]{});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(timeout, [=] {
        (*cancelRequestOnTimeout)();
    }, TimerMode::Once);
    auto onResponseReceived = [=, &responseTimeoutTimer](const std::optional<fcgi::ResponseData>& fcgiResponse) {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
            if (!fcgiResponse->errorMsg.empty())
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(fastcgi::Response{fcgiResponse->data, fcgiResponse->errorMsg});
        }
        else
            responseHandler(std::nullopt);
    };
    auto& clientConnection = tcpClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::ip::tcp>>(io_, errorHandler_));
    auto address = asio::ip::make_address(ipAddress);
    clientConnection->makeRequest(
            asio::ip::tcp::endpoint{address, port},
            std::move(request),
            onResponseReceived,
            cancelRequestOnTimeout);
}


void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler)
{
    makeRequest(
            ipAddress,
            port,
            request,
            responseHandler,
            std::chrono::seconds{3});
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler,
        const std::chrono::milliseconds& timeout)
{
    auto cancelRequestOnTimeout = std::make_shared<std::function<void()>>([]{});
    auto& responseTimeoutTimer = timerProvider_.emplaceTimer();
    responseTimeoutTimer.start(timeout, [=] {
        (*cancelRequestOnTimeout)();
    }, TimerMode::Once);

    auto onResponseReceived = [=, &responseTimeoutTimer](const std::optional<fcgi::ResponseData>& fcgiResponse) {
        responseTimeoutTimer.stop();
        if (fcgiResponse) {
             if (!fcgiResponse->errorMsg.empty())
                errorHandler_(ErrorType::RequestProcessingError, -1, fcgiResponse->errorMsg);
            responseHandler(http::responseFromString(fcgiResponse->data));
        }
        else
            responseHandler(std::nullopt);
    };
    auto& clientConnection = tcpClientConnections_.emplace_back(
            std::make_unique<ClientConnection<asio::ip::tcp>>(io_, errorHandler_));
    auto [params, stdIn] = request.toFcgiData(http::FormType::Multipart);
    auto fcgiRequest = fastcgi::Request{std::move(params), std::move(stdIn)};
    auto address = asio::ip::make_address(ipAddress);
    clientConnection->makeRequest(
            asio::ip::tcp::endpoint{address, port},
            std::move(fcgiRequest),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void Client::disconnect()
{
    localClientConnections_.clear();
}

}