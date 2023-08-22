#include "connectionfactory.h"
#include "connection.h"
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#else
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#endif
#include <asyncgi/detail/external/sfun/interface.h>
#include <memory>

namespace asyncgi::detail {

ConnectionFactory::ConnectionFactory(
        RequestProcessor requestProcessor,
        IOService& ioService,
        EventHandlerProxy& eventHandler)
    : requestProcessor_{std::move(requestProcessor)}
    , ioService_{ioService}
    , eventHandler_{eventHandler}
{
}

template<typename TProtocol>
std::shared_ptr<Connection<TProtocol>> ConnectionFactory::makeConnection()
{
    return std::make_shared<Connection<TProtocol>>(
            requestProcessor_,
            ioService_.get().nextIO(),
            eventHandler_,
            sfun::access_token<ConnectionFactory>{});
}

template class std::shared_ptr<Connection<asio::local::stream_protocol>> ConnectionFactory::makeConnection<
        asio::local::stream_protocol>();
template class std::shared_ptr<Connection<asio::ip::tcp>> ConnectionFactory::makeConnection<asio::ip::tcp>();

} // namespace asyncgi::detail
