#include "connection.h"
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/write.hpp>
#include <asyncgi/request.h>
#include <asyncgi/response.h>
#include <memory>

namespace asyncgi::detail {

template <typename TProtocol>
Connection<TProtocol>::Connection(
        RequestProcessor requestProcessor,
        asio::io_context& io,
        ErrorHandlerFunc errorHandler,
        sfun::AccessPermission<ConnectionFactory>)
    : requestProcessor_{std::move(requestProcessor)}
    , asioDispatcher_{io}
    , timerProvider_{io}
    , client_{io, errorHandler}
    , socket_{io}
    , errorHandler_{errorHandler}
{
}

template <typename TProtocol>
asio::basic_socket<TProtocol>& Connection<TProtocol>::socket()
{
    return socket_;
}

template <typename TProtocol>
void Connection<TProtocol>::process()
{
    socket_.async_read_some(
            asio::buffer(buffer_),
            [self = this->shared_from_this(), this](const auto& error, auto bytesRead)
            {
                if (error) {
                    if (error.value() != asio::error::operation_aborted)
                        errorHandler_(ErrorType::SocketReadError, error);
                    return;
                }
                self->readData(bytesRead);
            });
}

template <typename TProtocol>
void Connection<TProtocol>::readData(std::size_t bytesRead)
{
    fcgi::Responder::receiveData(buffer_.data(), bytesRead);
    process();
}

template <typename TProtocol>
void Connection<TProtocol>::sendData(const std::string& data)
{
    if (bytesToWrite_) {
        nextWriteBuffer_ += data;
        return;
    }

    if (!nextWriteBuffer_.empty()) {
        writeBuffer_ = nextWriteBuffer_ + data;
        nextWriteBuffer_.clear();
    }
    else
        writeBuffer_ = data;

    bytesToWrite_ += writeBuffer_.size();
    asio::async_write(
            socket_,
            asio::buffer(writeBuffer_),
            [self = this->shared_from_this(), this](const auto& error, auto bytesWritten)
            {
                if (error) {
                    errorHandler_(ErrorType::SocketWriteError, error);
                    return;
                }
                self->onBytesWritten(bytesWritten);
            });
}

template <typename TProtocol>
void Connection<TProtocol>::onBytesWritten(std::size_t numOfBytes)
{
    bytesToWrite_ -= numOfBytes;
    if (bytesToWrite_)
        return;

    if (!nextWriteBuffer_.empty())
        sendData({});
    else if (disconnectRequested_)
        close();
}

template <typename TProtocol>
void Connection<TProtocol>::disconnect()
{
    disconnectRequested_ = true;
    if (bytesToWrite_ > 0)
        return;
    close();
}

template <typename TProtocol>
void Connection<TProtocol>::processRequest(fcgi::Request&& fcgiRequest, fcgi::Response&& fcgiResponse)
{
    try {
        fcgiRequest_ = std::move(fcgiRequest);
        responseSender_.emplace(std::move(fcgiResponse));
        const auto request = Request{*fcgiRequest_};
        auto response = ResponseContext{*responseSender_, timerProvider_, client_, asioDispatcher_};
        requestProcessor_(request, response);
    } catch (const std::exception& e) {
        errorHandler_(ErrorType::RequestProcessingError, -1, e.what());
    }
}

template <typename TProtocol>
void Connection<TProtocol>::close()
{
    try {
        socket_.shutdown(TProtocol::socket::shutdown_both);
        socket_.close();
    } catch (const std::system_error& e) {
        errorHandler_(ErrorType::SocketCloseError, e.code());
    }
    disconnectRequested_ = false;
}

template class Connection<asio::local::stream_protocol>;
template class Connection<asio::ip::tcp>;

} // namespace asyncgi::detail
