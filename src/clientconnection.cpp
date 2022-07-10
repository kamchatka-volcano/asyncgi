#include "client.h"
#include "timerprovider.h"
#include <asio/write.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/ip/tcp.hpp>

namespace asyncgi::detail{

template<typename TProtocol>
ClientConnection<TProtocol>::ClientConnection(asio::io_context& io, ErrorHandlerFunc errorHandler)
    : socket_{io}
    , errorHandler_{std::move(errorHandler)}
{}

template<typename TProtocol>
ClientConnection<TProtocol>::~ClientConnection()
{
    close();
}

template<typename TProtocol>
void ClientConnection<TProtocol>::makeRequest(
        const typename TProtocol::endpoint& socketPath,
        std::map<std::string, std::string> fcgiParams,
        std::string fcgiStdIn,
        std::function<void(const std::optional<fcgi::ResponseData>&)> responseHandler,
        const std::shared_ptr<std::function<void()>>& cancelRequestOnTimeout)
{
    socket_.async_connect(socketPath,
        [this, cancelRequestOnTimeout,
         fcgiParams = std::move(fcgiParams),
         fcgiStdIn = std::move(fcgiStdIn),
         responseHandler = std::move(responseHandler)](auto error_code) mutable{
            if (error_code){
                responseHandler(std::nullopt);
                return;
            }
            processReading();
            auto requestHandle = sendRequest(std::move(fcgiParams), std::move(fcgiStdIn), responseHandler);
            if (requestHandle)
                *cancelRequestOnTimeout = [=]() mutable{
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
    asio::async_write(socket_, asio::buffer(writeBuffer_),
        [this](const auto& error, auto bytesWritten){
            if (error){
                errorHandler_(ErrorType::SocketWriteError, error);
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
    try{
        socket_.shutdown(asio::basic_stream_socket<TProtocol>::shutdown_both);
        socket_.close();
    }
    catch(const std::system_error& e){
        errorHandler_(ErrorType::SocketCloseError, e.code());
    }
    disconnectRequested_ = false;
}

template class ClientConnection<asio::local::stream_protocol>;
template class ClientConnection<asio::ip::tcp>;

}