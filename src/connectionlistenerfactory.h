#pragma once
#include "connectionlistener.h"
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/eventhandlerproxy.h>
#include <asyncgi/detail/external/sfun/member.h>
#include <asyncgi/errors.h>
#include <memory>

namespace ASYNCGI_ASIO {
class io_context;
}

namespace asyncgi::detail {
class ConnectionFactory;

class ConnectionListenerFactory {
public:
    ConnectionListenerFactory(asio::io_context&, std::unique_ptr<ConnectionFactory>, EventHandlerProxy&);
    ~ConnectionListenerFactory();
    ConnectionListenerFactory(const ConnectionListenerFactory&) = delete;
    ConnectionListenerFactory& operator=(const ConnectionListenerFactory&) = delete;
    ConnectionListenerFactory(ConnectionListenerFactory&&) noexcept = default;
    ConnectionListenerFactory& operator=(ConnectionListenerFactory&&) noexcept = default;

    template<typename TProtocol>
    std::unique_ptr<ConnectionListener<TProtocol>> makeConnectionListener(const typename TProtocol::endpoint& address);

private:
    sfun::member<asio::io_context&> io_;
    std::unique_ptr<ConnectionFactory> connectionFactory_;
    sfun::member<EventHandlerProxy&> eventHandler_;
};

} // namespace asyncgi::detail
