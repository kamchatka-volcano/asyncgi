#pragma once
#include "accesspermission.h"
#include "asiodispatcher.h"
#include "timerprovider.h"
#include "client.h"
#include <asyncgi/requestprocessor.h>
#include <asyncgi/errors.h>
#include <asio/basic_stream_socket.hpp>
#include <fcgi_responder/request.h>
#include <fcgi_responder/response.h>
#include <fcgi_responder/responder.h>
#include <fcgi_responder/fcgi_limits.h>
#include <memory>
#include <array>
#include <optional>

namespace asio{
    class io_context;
}

namespace asyncgi::detail{
class ConnectionFactory;

template <typename TProtocol>
class Connection : public std::enable_shared_from_this<Connection<TProtocol>>, public fcgi::Responder {
public:
    Connection(IRequestProcessor&, asio::io_context&, ErrorHandlerFunc, AccessPermission<ConnectionFactory>);
    asio::basic_socket<TProtocol>& socket();
    void process();
    void readData(std::size_t bytesRead);
    void sendData(const std::string& data) final;
    void disconnect() final;
    void processRequest(fcgi::Request&& request, fcgi::Response&& response) final;

private:
    void onBytesWritten(std::size_t numOfBytes);
    void close();

private:
    std::optional<fcgi::Request> fcgiRequest_;
    std::optional<fcgi::Response> fcgiResponse_;
    IRequestProcessor& requestProcessor_;
    AsioDispatcher asioDispatcher_;
    TimerProvider timerProvider_;
    Client client_;
    asio::basic_stream_socket<TProtocol> socket_;
    std::array<char, fcgi::maxRecordSize> buffer_;
    std::string writeBuffer_;
    std::string nextWriteBuffer_;
    std::size_t bytesToWrite_ = 0;
    bool disconnectRequested_ = false;
    ErrorHandler errorHandler_;
};

}
