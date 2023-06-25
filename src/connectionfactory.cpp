#include "connectionfactory.h"
#include "connection.h"
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asyncgi/detail/external/sfun/interface.h>
#include <memory>

namespace asyncgi::detail {

ConnectionFactory::ConnectionFactory(
        RequestProcessor requestProcessor,
        IOService& ioService,
        ErrorHandler& errorHandler)
    : requestProcessor_{std::move(requestProcessor)}
    , ioService_{ioService}
    , errorHandler_{errorHandler}
{
}

template<typename TProtocol>
std::shared_ptr<Connection<TProtocol>> ConnectionFactory::makeConnection()
{
    return std::make_shared<Connection<TProtocol>>(
            requestProcessor_,
            ioService_.get().nextIO(),
            errorHandler_,
            sfun::access_token<ConnectionFactory>{});
}

template class std::shared_ptr<Connection<asio::local::stream_protocol>> ConnectionFactory::makeConnection<
        asio::local::stream_protocol>();
template class std::shared_ptr<Connection<asio::ip::tcp>> ConnectionFactory::makeConnection<asio::ip::tcp>();

} // namespace asyncgi::detail
