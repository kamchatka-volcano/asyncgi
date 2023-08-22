#include "clientconnection.h"
#include "asio_error.h"
#include "clientservice.h"
#include "timerprovider.h"
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/write.hpp>
#else
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/write.hpp>
#endif

namespace asyncgi::detail {

template<typename TProtocol>
ClientConnection<TProtocol>::ClientConnection(asio::io_context& io, EventHandlerProxy& eventHandler)
    : socket_{io}
    , eventHandler_{eventHandler}
{
}

template<typename TProtocol>
ClientConnection<TProtocol>::~ClientConnection()
{
    close();
}

template<typename TProtocol>
void ClientConnection<TProtocol>::makeRequest(
        const typename TProtocol::endpoint& socketPath,
        fastcgi::Request request,
        std::function<void(std::optional<fcgi::ResponseData>)> responseHandler,
        const std::shared_ptr<std::function<void()>>& cancelRequestOnTimeout)
{
    socket_.async_connect(
            socketPath,
            [this,
             cancelRequestOnTimeout,
             fcgiParams = std::move(request.params),
             fcgiStdIn = std::move(request.stdIn),
             responseHandler = std::move(responseHandler)](auto error_code) mutable
            {
                if (error_code) {
                    responseHandler(std::nullopt);
                    return;
                }
                processReading();
                auto requestHandle = sendRequest(std::move(fcgiParams), std::move(fcgiStdIn), responseHandler);
                if (requestHandle)
                    *cancelRequestOnTimeout = [=]() mutable
                    {
                        requestHandle->cancelRequest();
                    };
                else
                    responseHandler(std::nullopt);
            });
}

template<typename TProtocol>
void ClientConnection<TProtocol>::disconnect()
{
    disconnectRequested_ = true;
    if (bytesToWrite_ > 0)
        return;
    close();
}

template<typename TProtocol>
void ClientConnection<TProtocol>::processReading()
{
    if (!socket_.is_open())
        return;
    socket_.async_read_some(
            asio::buffer(buffer_),
            [this](const auto& error, auto bytesRead)
            {
                if (error) {
                    if (error.value() != asio::error::operation_aborted)
                        eventHandler_(ErrorEvent::SocketReadError, error.message());
                    return;
                }
                readData(bytesRead);
            });
}

template<typename TProtocol>
void ClientConnection<TProtocol>::readData(std::size_t bytesRead)
{
    fcgi::Requester::receiveData(buffer_.data(), bytesRead);
    processReading();
}

template<typename TProtocol>
void ClientConnection<TProtocol>::sendData(const std::string& data)
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
            [this](const auto& error, auto bytesWritten)
            {
                if (error) {
                    eventHandler_(ErrorEvent::SocketWriteError, error.message());
                    return;
                }
                onBytesWritten(bytesWritten);
            });
}

template<typename TProtocol>
void ClientConnection<TProtocol>::onBytesWritten(std::size_t numOfBytes)
{
    bytesToWrite_ -= numOfBytes;
    if (bytesToWrite_)
        return;

    if (!nextWriteBuffer_.empty())
        sendData({});
    else if (disconnectRequested_)
        close();
}

template<typename TProtocol>
void ClientConnection<TProtocol>::close()
{
    auto error = asio_error{};
    socket_.shutdown(asio::basic_stream_socket<TProtocol>::shutdown_both, error);
    socket_.close(error);
    if (error) {
        eventHandler_(ErrorEvent::SocketCloseError, error.message());
    }
    disconnectRequested_ = false;
}

template class ClientConnection<asio::local::stream_protocol>;
template class ClientConnection<asio::ip::tcp>;

} // namespace asyncgi::detail