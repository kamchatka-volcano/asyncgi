#include "connection.h"
#include "requestcontext.h"
#include <asyncgi/request.h>
#include <asyncgi/response.h>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <memory>

namespace asyncgi{

Connection::Connection(RequestProcessor& requestProcessor, asio::io_context& io, ErrorHandlerFunc errorHandler, ConnectionFactoryTag)
    : requestProcessor_(requestProcessor)
    , timer_(io)
    , socket_(io)
    , errorHandler_(errorHandler)
{}

unixdomain::socket& Connection::socket()
{
    return socket_;
}

void Connection::process()
{
    socket_.async_read_some(asio::buffer(buffer_),
        [self = shared_from_this(), this](const auto& error, auto bytesRead){
            if (error){
                if (error.value() != asio::error::operation_aborted)
                    errorHandler_(ErrorType::SocketReadError, error);
                return;
            }
            self->readData(bytesRead);
        });
}

void Connection::readData(std::size_t bytesReaded)
{
    fcgi::Responder::receiveData(buffer_.data(), bytesReaded);    
    process();
}

void Connection::sendData(const std::string& data)
{
    bytesToWrite_ += data.size();
    writeBuffer_ = data;
    asio::async_write(socket_, asio::buffer(writeBuffer_),
        [self = shared_from_this(), this](const auto& error, auto bytesWritten){
            if (error){
                errorHandler_(ErrorType::SocketWriteError, error);
                return;
            }
            self->onBytesWritten(bytesWritten);
        });
}

void Connection::onBytesWritten(std::size_t numOfBytes)
{
    bytesToWrite_ -= numOfBytes;
    if (!bytesToWrite_ && disconnectRequested_)
        close();
}

void Connection::disconnect()
{
    disconnectRequested_ = true;
    if (bytesToWrite_ > 0)
        return;
    close();
}

void Connection::processRequest(fcgi::Request&& fcgiRequest, fcgi::Response&& fcgiResponse)
{
    try{
        auto context = std::make_shared<RequestContext>(std::move(fcgiRequest),
                                                        std::move(fcgiResponse));
        const auto request = Request{context};
        auto response = Response{context, timer_};
        requestProcessor_.process(request, response);
    }
    catch(const std::exception& e)
    {
        errorHandler_(ErrorType::RequestProcessingError, -1, e.what());
    }
}

void Connection::close()
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
