#pragma once
#include "types.h"
#include "errors.h"
#include <filesystem>
#include <string>
#include <memory>

namespace asio{
class io_context;
}

namespace asyncgi{
class ConnectionProcessor;
class ConnectionFactory;

class Server{
public:
    Server(asio::io_context&, std::unique_ptr<ConnectionFactory>, ErrorHandlerFunc);
    ~Server();
    void listen(const std::filesystem::path& socketPath);

private:    
    void initUnixDomainSocket(const std::filesystem::path& path);

private:    
    asio::io_context& io_;
    std::unique_ptr<ConnectionFactory> connectionFactory_;
    ErrorHandlerFunc errorHandler_;
    std::unique_ptr<ConnectionProcessor> connectionProcessor_;
};

}
