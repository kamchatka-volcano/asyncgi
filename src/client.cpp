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
        std::map<std::string, std::string> fcgiParams,
        std::string fcgiStdIn,
        std::function<void(const std::optional<std::string>&)> responseHandler)
{
    makeRequest(
            socketPath,
            std::move(fcgiParams),
            std::move(fcgiStdIn),
            std::move(responseHandler),
            std::chrono::seconds{3});
}

void Client::makeRequest(
        const fs::path& socketPath,
        std::map<std::string, std::string> fcgiParams,
        std::string fcgiStdIn,
        std::function<void(const std::optional<std::string>&)> responseHandler,
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
            responseHandler(fcgiResponse->data);
        }
        else
            responseHandler(std::nullopt);
    };
    auto& clientProcessor = localClientProcessors_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, errorHandler_));
    clientProcessor->makeRequest(
            asio::local::stream_protocol::endpoint{socketPath},
            std::move(fcgiParams),
            std::move(fcgiStdIn),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void Client::makeRequest(
        const fs::path& socketPath,
        const http::Request& request,
        const std::function<void(const std::optional<http::Response>&)>& responseHandler)
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
        const std::function<void(const std::optional<http::Response>&)>& responseHandler,
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
    auto& clientProcessor = localClientProcessors_.emplace_back(
            std::make_unique<ClientConnection<asio::local::stream_protocol>>(io_, errorHandler_));
    auto fcgiRequest = request.toFcgiData(http::FormType::Multipart);
    clientProcessor->makeRequest(
            asio::local::stream_protocol::endpoint{socketPath},
            std::move(fcgiRequest.params),
            std::move(fcgiRequest.stdIn),
            onResponseReceived,
            cancelRequestOnTimeout);
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        std::map<std::string, std::string> fcgiParams,
        std::string fcgiStdIn,
        std::function<void(const std::optional<std::string>&)> responseHandler)
{
    makeRequest(
            ipAddress,
            port,
            std::move(fcgiParams),
            std::move(fcgiStdIn),
            std::move(responseHandler),
            std::chrono::seconds{3});
}

void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        std::map<std::string, std::string> fcgiParams,
        std::string fcgiStdIn,
        std::function<void(const std::optional<std::string>&)> responseHandler,
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
            responseHandler(fcgiResponse->data);
        }
        else
            responseHandler(std::nullopt);
    };
    auto& clientProcessor = tcpClientProcessors_.emplace_back(
            std::make_unique<ClientConnection<asio::ip::tcp>>(io_, errorHandler_));
    auto address = asio::ip::make_address(ipAddress);
    clientProcessor->makeRequest(
            asio::ip::tcp::endpoint{address, port},
            std::move(fcgiParams),
            std::move(fcgiStdIn),
            onResponseReceived,
            cancelRequestOnTimeout);
}


void Client::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(const std::optional<http::Response>&)>& responseHandler)
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
        const std::function<void(const std::optional<http::Response>&)>& responseHandler,
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
    auto& clientProcessor = tcpClientProcessors_.emplace_back(
            std::make_unique<ClientConnection<asio::ip::tcp>>(io_, errorHandler_));
    auto fcgiRequest = request.toFcgiData(http::FormType::Multipart);
    auto address = asio::ip::make_address(ipAddress);
    clientProcessor->makeRequest(
            asio::ip::tcp::endpoint{address, port},
            std::move(fcgiRequest.params),
            std::move(fcgiRequest.stdIn), onResponseReceived, cancelRequestOnTimeout);
}

void Client::disconnect()
{
    localClientProcessors_.clear();
}

}