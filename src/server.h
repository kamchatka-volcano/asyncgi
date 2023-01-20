#pragma once
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asyncgi/errors.h>
#include <asyncgi/iserver.h>
#include <asyncgi/types.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace asyncgi::detail {
class ConnectionListenerFactory;
template<typename TProtocol>
class ConnectionListener;

class Server : public IServer {
public:
    explicit Server(std::unique_ptr<ConnectionListenerFactory>);
    ~Server() override;

    void listen(const std::filesystem::path& socketPath) override;
    void listen(std::string_view ipAddress, uint16_t portNumber) override;

private:
    std::unique_ptr<ConnectionListenerFactory> connectionListenerFactory_;
    std::vector<std::unique_ptr<ConnectionListener<asio::local::stream_protocol>>> localConnectionProcessors_;
    std::vector<std::unique_ptr<ConnectionListener<asio::ip::tcp>>> tcpConnectionProcessors_;
};

} // namespace asyncgi::detail
