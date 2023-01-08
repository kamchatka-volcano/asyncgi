#pragma once
#include "clientconnection.h"
#include "timerprovider.h"
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asyncgi/errors.h>
#include <asyncgi/iclient.h>
#include <asyncgi/itimer.h>
#include <fcgi_responder/requester.h>
#include <hot_teacup/request.h>
#include <hot_teacup/response_view.h>
#include <filesystem>
#include <memory>
#include <vector>

namespace asyncgi::detail {
namespace fs = std::filesystem;
class TimerProvider;

class Client : public IClient {
public:
    Client(asio::io_context&, ErrorHandlerFunc);
    void makeRequest(
            const fs::path& socketPath,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler) override;
    void makeRequest(
            const fs::path& socketPath,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            const std::chrono::milliseconds& timeout) override;
    void makeRequest(
            const fs::path& socketPath,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler) override;
    void makeRequest(
            const fs::path& socketPath,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            const std::chrono::milliseconds& timeout) override;

    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler) override;
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            const std::chrono::milliseconds& timeout) override;
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler) override;
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            const std::chrono::milliseconds& timeout) override;

    void disconnect() override;

private:
    asio::io_context& io_;
    TimerProvider timerProvider_;
    ErrorHandler errorHandler_;
    std::vector<std::unique_ptr<ClientConnection<asio::local::stream_protocol>>> localClientConnections_;
    std::vector<std::unique_ptr<ClientConnection<asio::ip::tcp>>> tcpClientConnections_;
};

} // namespace asyncgi::detail
