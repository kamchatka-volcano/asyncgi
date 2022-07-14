#pragma once
#include "timerprovider.h"
#include "clientconnection.h"
#include "timerprovider.h"
#include <asyncgi/iclient.h>
#include <asyncgi/itimer.h>
#include <asyncgi/errors.h>
#include <hot_teacup/request.h>
#include <hot_teacup/response.h>
#include <fcgi_responder/requester.h>
#include <asio/local/stream_protocol.hpp>
#include <asio/ip/tcp.hpp>
#include <filesystem>
#include <memory>
#include <vector>

namespace asyncgi::detail{
namespace fs = std::filesystem;
class TimerProvider;

class Client : public IClient {
public:
    Client(asio::io_context&, ErrorHandlerFunc);
    void makeRequest(
            const fs::path& socketPath,
            std::map<std::string, std::string> fcgiParams,
            std::string fcgiStdIn,
            std::function<void(const std::optional<std::string>&)> responseHandler) override;
    void makeRequest(
            const fs::path& socketPath,
            std::map<std::string, std::string> fcgiParams,
            std::string fcgiStdIn,
            std::function<void(const std::optional<std::string>&)> responseHandler,
            const std::chrono::milliseconds& timeout) override;
    void makeRequest(
            const fs::path& socketPath,
            const http::Request& request,
            const std::function<void(const std::optional<http::Response>&)>& responseHandler) override;
    void makeRequest(
            const fs::path& socketPath,
            const http::Request& request,
            const std::function<void(const std::optional<http::Response>&)>& responseHandler,
            const std::chrono::milliseconds& timeout) override;

     void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            std::map<std::string, std::string> fcgiParams,
            std::string fcgiStdIn,
            std::function<void(const std::optional<std::string>&)> responseHandler) override;
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            std::map<std::string, std::string> fcgiParams,
            std::string fcgiStdIn,
            std::function<void(const std::optional<std::string>&)> responseHandler,
            const std::chrono::milliseconds& timeout) override;
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(const std::optional<http::Response>&)>& responseHandler) override;
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(const std::optional<http::Response>&)>& responseHandler,
            const std::chrono::milliseconds& timeout) override;

    void disconnect() override;

private:
    asio::io_context& io_;
    TimerProvider timerProvider_;
    ErrorHandler errorHandler_;
    std::vector<std::unique_ptr<ClientConnection<asio::local::stream_protocol>>> localClientProcessors_;
    std::vector<std::unique_ptr<ClientConnection<asio::ip::tcp>>> tcpClientProcessors_;
};

}