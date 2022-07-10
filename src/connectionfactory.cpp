#include "connectionfactory.h"
#include "connection.h"
#include "iruntime.h"
#include <asio/local/stream_protocol.hpp>
#include <asio/ip/tcp.hpp>
#include <memory>

namespace asyncgi::detail{

ConnectionFactory::ConnectionFactory(IRequestProcessor& requestProcessor, IRuntime& runtime, ErrorHandlerFunc errorHandler)
    : requestProcessor_{requestProcessor}
    , runtime_{runtime}
    , errorHandler_{std::move(errorHandler)}
{}

template<typename TProtocol>
std::shared_ptr<Connection<TProtocol>> ConnectionFactory::makeConnection()
{
    return std::make_shared<Connection<TProtocol>>(requestProcessor_, runtime_.nextIO(), errorHandler_, AccessPermission<ConnectionFactory>{});
}

template class std::shared_ptr<Connection<asio::local::stream_protocol>> ConnectionFactory::makeConnection<asio::local::stream_protocol>();
template class std::shared_ptr<Connection<asio::ip::tcp>> ConnectionFactory::makeConnection<asio::ip::tcp>();

}

