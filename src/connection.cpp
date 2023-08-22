#include "connection.h"
#include "asio_error.h"
#include "responsecontext.h"
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
#include <asyncgi/request.h>
#include <asyncgi/response.h>
#include <memory>

namespace asyncgi::detail {

template<typename TProtocol>
Connection<TProtocol>::Connection(
        RequestProcessor requestProcessor,
        asio::io_context& io,
        EventHandlerProxy& eventHandler,
        sfun::access_permission<ConnectionFactory>)
    : requestProcessor_{std::move(requestProcessor)}
    , asioDispatcher_{io}
    , timerProvider_{io}
    , client_{io, eventHandler}
    , socket_{io}
    , eventHandler_{eventHandler}
{
}

template<typename TProtocol>
Connection<TProtocol>::~Connection() = default;

template<typename TProtocol>
asio::basic_socket<TProtocol>& Connection<TProtocol>::socket()
{
    return socket_;
}

template<typename TProtocol>
void Connection<TProtocol>::process()
{
    socket_.async_read_some(
            asio::buffer(buffer_),
            [self = this->shared_from_this(), this](const auto& error, auto bytesRead)
            {
                if (error) {
                    if (error.value() != asio::error::operation_aborted) {
                        eventHandler_(ErrorEvent::SocketReadError, error.message());
                        close();
                    }
                    return;
                }
                self->readData(bytesRead);
            });
}

template<typename TProtocol>
void Connection<TProtocol>::readData(std::size_t bytesRead)
{
    fcgi::Responder::receiveData(buffer_.data(), bytesRead);
    process();
}

template<typename TProtocol>
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
                    eventHandler_(ErrorEvent::SocketWriteError, error.message());
                    close();
                    return;
                }
                self->onBytesWritten(bytesWritten);
            });
}

template<typename TProtocol>
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

template<typename TProtocol>
void Connection<TProtocol>::disconnect()
{
    disconnectRequested_ = true;
    if (bytesToWrite_ > 0)
        return;
    close();
}

template<typename TProtocol>
void Connection<TProtocol>::processRequest(fcgi::Request&& fcgiRequest, fcgi::Response&& fcgiResponse)
{
    fcgiRequest_ = std::move(fcgiRequest);
    responseSender_.emplace(std::move(fcgiResponse));
    const auto request = Request{*fcgiRequest_};
    responseContext_ = std::make_shared<ResponseContext>(*responseSender_, timerProvider_, client_, asioDispatcher_);
    try {
        requestProcessor_(request, responseContext_);
    }
    catch (const std::exception& e) {
        auto response = Response{responseContext_};
        response.send(http::ResponseStatus::_500_Internal_Server_Error);
        eventHandler_(ErrorEvent::RequestProcessingError, e.what());
    }
    catch (...) {
        auto response = Response{responseContext_};
        response.send(http::ResponseStatus::_500_Internal_Server_Error);
        eventHandler_(ErrorEvent::RequestProcessingError, "Unknown error");
    }
}

template<typename TProtocol>
void Connection<TProtocol>::close()
{
    auto error = asio_error{};
    socket_.shutdown(TProtocol::socket::shutdown_both, error);
    socket_.close(error);

    if (error)
        eventHandler_(ErrorEvent::SocketCloseError, error.message());
    disconnectRequested_ = false;
}

template class Connection<asio::local::stream_protocol>;
template class Connection<asio::ip::tcp>;

} // namespace asyncgi::detail
