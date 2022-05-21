#include "client.h"
#include "alias_unixdomain.h"
#include <asio/write.hpp>

namespace asyncgi{

Client::Client(asio::io_context& io, ErrorHandlerFunc errorHandler)
    : socket_{io}
    , errorHandler_{std::move(errorHandler)}
{}

void Client::makeRequest(const fs::path& socketPath,
                         const FCGIRequest& fcgiRequest,
                         const std::function<void(const std::optional<FCGIResponse>&)>& responseHandler)
{
    auto convertResponseHandler = [responseHandler](const std::optional<fcgi::ResponseData>& fcgiResponse) {
        if (fcgiResponse)
            responseHandler(FCGIResponse{fcgiResponse->data, fcgiResponse->errorMsg});
        else
            responseHandler(std::nullopt);
    };

    if (isConnected_){
        sendRequest(fcgiRequest.params, fcgiRequest.stdIn, convertResponseHandler);
        return;
    }

    auto onFcgiConnectionEstablished =
        [=](bool isConnected) {
            if (!isConnected){
                responseHandler(std::nullopt);
                return;
            }
            isConnected_ = true;
            sendRequest(fcgiRequest.params, fcgiRequest.stdIn, convertResponseHandler);
        };

    socket_.async_connect(unixdomain::endpoint{socketPath},
        [=](auto error_code){
            if (error_code){
                responseHandler(std::nullopt);
                return;
            }
            processReading();
            initConnection(onFcgiConnectionEstablished, true);
        });
}

void Client::makeRequest(const fs::path& socketPath,
                         const http::Request& request,
                         const std::function<void(const std::optional<http::Response>&)>& responseHandler)
{
    auto convertResponseHandler = [responseHandler](const std::optional<fcgi::ResponseData>& fcgiResponse) {
        if (fcgiResponse)
            responseHandler(http::responseFromString(fcgiResponse->data));
        else
            responseHandler(std::nullopt);
    };

    auto fcgiRequest = request.toFcgiData(http::FormType::Multipart);
    if (isConnected_){
        sendRequest(fcgiRequest.params, fcgiRequest.stdIn, convertResponseHandler);
        return;
    }

    auto onFcgiConnectionEstablished =
        [=](bool isConnected) {
            if (!isConnected){
                responseHandler(std::nullopt);
                return;
            }
            isConnected_ = true;
            sendRequest(fcgiRequest.params, fcgiRequest.stdIn, convertResponseHandler);
        };

    socket_.async_connect(unixdomain::endpoint{socketPath},
        [=](auto error_code){
            if (error_code){
                responseHandler(std::nullopt);
                return;
            }
            processReading();
            initConnection(onFcgiConnectionEstablished, true);
        });
}


void Client::disconnect()
{
    if (!isConnected_)
        return;
    disconnectRequested_ = true;
    if (bytesToWrite_ > 0)
        return;
    close();
}

void Client::processReading()
{
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