#pragma once
#include <asyncgi/iserver.h>
#include <asyncgi/types.h>
#include <asyncgi/errors.h>
#include <filesystem>
#include <string>
#include <memory>

namespace asio{
class io_context;
}

namespace asyncgi::detail{
class ConnectionProcessor;
class ConnectionFactory;

class Server : public IServer{
public:
    Server(asio::io_context&, std::unique_ptr<detail::ConnectionFactory>, ErrorHandlerFunc);
    ~Server() override;
    void listen(const std::filesystem::path& socketPath) override;

private:    
    asio::io_context& io_;
    std::unique_ptr<detail::ConnectionFactory> connectionFactory_;
    ErrorHandlerFunc errorHandler_;
    std::unique_ptr<detail::ConnectionProcessor> connectionProcessor_;
};

}
