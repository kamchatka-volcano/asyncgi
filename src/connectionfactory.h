#pragma once
#include "ioservice.h"
#include <asyncgi/detail/eventhandlerproxy.h>
#include <asyncgi/detail/external/sfun/member.h>
#include <asyncgi/errors.h>
#include <asyncgi/requestprocessor.h>
#include <memory>

namespace asyncgi::detail {
template<typename TProtocol>
class Connection;

class ConnectionFactory {
public:
    ConnectionFactory(RequestProcessor requestProcessor, IOService& ioService, EventHandlerProxy& eventHandler);
    template<typename TProtocol>
    std::shared_ptr<Connection<TProtocol>> makeConnection();

private:
    RequestProcessor requestProcessor_;
    sfun::member<IOService&> ioService_;
    sfun::member<EventHandlerProxy&> eventHandler_;
};
} // namespace asyncgi::detail
