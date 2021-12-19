#include "connectionprocessor.h"
#include "connectionfactory.h"
#include "connection.h"
#include "iruntime.h"

namespace asyncgi::detail{

ConnectionProcessor::ConnectionProcessor(std::unique_ptr<unixdomain::acceptor> connectionListener,
                                         ConnectionFactory& connectionFactory,
                                         ErrorHandlerFunc errorHandler)
    : connectionListener_(std::move(connectionListener))
    , connectionFactory_(connectionFactory)
    , errorHandler_(errorHandler)

{
    waitForConnection();
}

void ConnectionProcessor::waitForConnection()
{
    auto connection = connectionFactory_.makeConnection();
    connectionListener_->async_accept(connection->socket(),
        [this, connection](auto error_code){
            onConnected(*connection, error_code);
        });
}

void ConnectionProcessor::onConnected(Connection& connection, const std::error_code& error)
{
    if (error){
        errorHandler_(ErrorType::ConnectionError, error);
        return;
    }
    connection.process();
    waitForConnection();
}

}
