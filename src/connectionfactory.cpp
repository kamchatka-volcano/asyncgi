#include "connectionfactory.h"
#include "connection.h"
#include "iruntime.h"

namespace asyncgi::detail{

ConnectionFactory::ConnectionFactory(IRequestProcessor& requestProcessor, IRuntime& runtime, ErrorHandlerFunc errorHandler)
    : requestProcessor_{requestProcessor}
    , runtime_{runtime}
    , errorHandler_{std::move(errorHandler)}
{}

std::shared_ptr<Connection> ConnectionFactory::makeConnection()
{
    return std::make_shared<Connection>(requestProcessor_, runtime_.nextIO(), errorHandler_, ConnectionFactoryTag{});
}

}

