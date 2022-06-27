#include "client.h"
#include "alias_unixdomain.h"
#include "timerprovider.h"
#include <asio/write.hpp>

namespace asyncgi::detail{

Client::Client(asio::io_context& io, ErrorHandlerFunc errorHandler)
    : socket_{io}
    , timerProvider_{io}
    , errorHandler_{std::move(errorHandler)}
{}

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

    makeRequest(socketPath, std::move(fcgiParams), std::move(fcgiStdIn), onResponseReceived, cancelRequestOnTimeout);
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
    auto fcgiRequest = request.toFcgiData(http::FormType::Multipart);
    makeRequest(socketPath, std::move(fcgiRequest.params), std::move(fcgiRequest.stdIn), onResponseReceived, cancelRequestOnTimeout);
}

void Client::makeRequest(
        const fs::path& socketPath,
        std::map<std::string, std::string> fcgiParams,
        std::string fcgiStdIn,
        std::function<void(const std::optional<fcgi::ResponseData>&)> responseHandler,
        const std::shared_ptr<std::function<void()>>& cancelRequestOnTimeout)
{
    socket_.async_connect(unixdomain::endpoint{socketPath},
        [this, cancelRequestOnTimeout,
         fcgiParams = std::move(fcgiParams),
         fcgiStdIn = std::move(fcgiStdIn),
         responseHandler = std::move(responseHandler)](auto error_code) mutable{
            if (error_code){
                responseHandler(std::nullopt);
                return;
            }
            processReading();
            auto requestHandle = sendRequest(std::move(fcgiParams), std::move(fcgiStdIn), std::move(responseHandler));
            if (requestHandle)
                *cancelRequestOnTimeout = [=]() mutable{
                    requestHandle->cancelRequest();
                };
            else
                responseHandler(std::nullopt);
        });
}

void Client::disconnect()
{
    disconnectRequested_ = true;
    if (bytesToWrite_ > 0)
        return;
    close();
}

void Client::processReading()
{
    if (!socket_.is_open())
        return;
    socket_.async_read_some(asio::buffer(buffer_),
        [this](const auto& error, auto bytesRead){
            if (error){
                if (error.value() != asio::error::operation_aborted)
                    errorHandler_(ErrorType::SocketReadError, error);
                return;
            }
            readData(bytesRead);
        });
}

void Client::readData(std::size_t bytesRead)
{
    fcgi::Requester::receiveData(buffer_.data(), bytesRead);
    processReading();
}


void Client::sendData(const std::string& data)
{
    bytesToWrite_ += data.size();
    writeBuffer_ = data;
    asio::async_write(socket_, asio::buffer(writeBuffer_),
        [this](const auto& error, auto bytesWritten){
            if (error){
                errorHandler_(ErrorType::SocketWriteError, error);
                return;
            }
            onBytesWritten(bytesWritten);
        });
}

void Client::onBytesWritten(std::size_t numOfBytes)
{
    bytesToWrite_ -= numOfBytes;
    if (!bytesToWrite_ && disconnectRequested_)
        close();
}

void Client::close()
{
    try{
        socket_.shutdown(unixdomain::socket::shutdown_both);
        socket_.close();
    }
    catch(const std::system_error& e){
        errorHandler_(ErrorType::SocketCloseError, e.code());
    }
    disconnectRequested_ = false;
}

}