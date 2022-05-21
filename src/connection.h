#pragma once
#include "alias_unixdomain.h"
#include "timer.h"
#include "client.h"
#include <asyncgi/requestprocessor.h>
#include <asyncgi/errors.h>
#include <fcgi_responder/responder.h>
#include <memory>
#include <array>

namespace asio{
    class io_context;
}

namespace asyncgi::detail{
struct ConnectionFactoryTag{
private:
    ConnectionFactoryTag() = default;
    friend class ConnectionFactory;
};

class Connection : public std::enable_shared_from_this<Connection>, public fcgi::Responder {
public:    
    Connection(IRequestProcessor&, asio::io_context&, ErrorHandlerFunc, ConnectionFactoryTag);
    unixdomain::socket& socket();
    void process();
    void readData(std::size_t bytesReaded);
    void sendData(const std::string& data) final;
    void disconnect() final;
    void processRequest(fcgi::Request&& request, fcgi::Response&& response) final;

private:
    void onBytesWritten(std::size_t numOfBytes);
    void close();

private:
    IRequestProcessor& requestProcessor_;
    Timer timer_;
    Client client_;
    unixdomain::socket socket_;
    std::array<char, 65536> buffer_;
    std::string writeBuffer_;
    std::size_t bytesToWrite_ = 0;
    bool disconnectRequested_ = false;
    ErrorHandler errorHandler_;
};

}
