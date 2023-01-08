#include "connectionfactory.h"
#include "connection.h"
#include "iruntime.h"
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asyncgi/detail/external/sfun/interface.h>
#include <memory>

namespace asyncgi::detail {

ConnectionFactory::ConnectionFactory(
        RequestProcessor requestProcessor,
        IRuntime& runtime,
        ErrorHandlerFunc errorHandler)
    : requestProcessor_{std::move(requestProcessor)}
    , runtime_{runtime}
    , errorHandler_{std::move(errorHandler)}
{
}

template <typename TProtocol>
std::shared_ptr<Connection<TProtocol>> ConnectionFactory::makeConnection()
{
    return std::make_shared<Connection<TProtocol>>(
            requestProcessor_,
            runtime_.nextIO(),
            errorHandler_,
            sfun::AccessToken<ConnectionFactory>{});
}

template class std::shared_ptr<Connection<asio::local::stream_protocol>> ConnectionFactory::makeConnection<
        asio::local::stream_protocol>();
template class std::shared_ptr<Connection<asio::ip::tcp>> ConnectionFactory::makeConnection<asio::ip::tcp>();

} // namespace asyncgi::detail
