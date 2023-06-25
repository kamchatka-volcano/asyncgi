#pragma once
#include "clientconnection.h"
#include "timerprovider.h"
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asyncgi/detail/external/sfun/member.h>
#include <asyncgi/errors.h>
#include <fcgi_responder/requester.h>
#include <hot_teacup/request.h>
#include <hot_teacup/response_view.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace asyncgi::detail {

class IOService;

class ClientService {
public:
    ClientService(asio::io_context&, ErrorHandler&);
    void makeRequest(
            const std::filesystem::path& socketPath,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});
    void makeRequest(
            const std::filesystem::path& socketPath,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void disconnect();

private:
    sfun::member<asio::io_context&> io_;
    sfun::member<ErrorHandler&> errorHandler_;
    TimerProvider timerProvider_;
    std::vector<std::unique_ptr<ClientConnection<asio::local::stream_protocol>>> localClientConnections_;
    std::vector<std::unique_ptr<ClientConnection<asio::ip::tcp>>> tcpClientConnections_;
};

} // namespace asyncgi::detail
