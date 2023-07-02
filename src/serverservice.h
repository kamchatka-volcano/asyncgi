#pragma once
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asyncgi/errors.h>
#include <asyncgi/types.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace asyncgi::detail {
class ConnectionListenerFactory;
template<typename TProtocol>
class ConnectionListener;

class ServerService {
public:
    explicit ServerService(std::unique_ptr<ConnectionListenerFactory>);
    ~ServerService();
    ServerService(const ServerService&) = delete;
    ServerService& operator=(const ServerService&) = delete;
    ServerService(ServerService&&) noexcept = default;
    ServerService& operator=(ServerService&&) noexcept = default;

    void listen(const std::filesystem::path& socketPath);
    void listen(std::string_view ipAddress, int portNumber);

private:
    std::unique_ptr<ConnectionListenerFactory> connectionListenerFactory_;
    std::vector<std::unique_ptr<ConnectionListener<asio::local::stream_protocol>>> localConnectionProcessors_;
    std::vector<std::unique_ptr<ConnectionListener<asio::ip::tcp>>> tcpConnectionProcessors_;
};

} // namespace asyncgi::detail
