#include "connectionfactory.h"
#include "connection.h"
#include <asyncgi/iruntime.h>

namespace asyncgi{

ConnectionFactory::ConnectionFactory(RequestProcessor& requestProcessor, asyncgi::IRuntime& runtime, ErrorHandlerFunc errorHandler)
    : requestProcessor_(requestProcessor)
    , runtime_(runtime)
    , errorHandler_(errorHandler)
{}

std::shared_ptr<Connection> ConnectionFactory::makeConnection()
{
    return std::make_shared<Connection>(requestProcessor_, runtime_.nextIO(), errorHandler_, ConnectionFactoryTag{});
}

}

