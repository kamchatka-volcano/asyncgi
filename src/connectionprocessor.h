#pragma once
#include "alias_unixdomain.h"
#include <asyncgi/errors.h>
#include <string>

namespace asyncgi{
class Connection;
class ConnectionFactory;

class ConnectionProcessor{
public:
    ConnectionProcessor(std::unique_ptr<unixdomain::acceptor> connectionListener,
                        ConnectionFactory& connectionFactory,
                        ErrorHandlerFunc errorHandler);

private:
    void waitForConnection();
    void onConnected(std::shared_ptr<Connection> connection, const std::error_code& error);

private:
    std::unique_ptr<unixdomain::acceptor> connectionListener_;
    ConnectionFactory& connectionFactory_;
    ErrorHandler errorHandler_;
};

}

