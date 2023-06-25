#pragma once
#include "ioservice.h"
#include <asyncgi/detail/external/sfun/member.h>
#include <asyncgi/errors.h>
#include <asyncgi/requestprocessor.h>
#include <memory>

namespace asyncgi::detail {
template<typename TProtocol>
class Connection;

class ConnectionFactory {
public:
    ConnectionFactory(RequestProcessor requestProcessor, IOService& ioService, ErrorHandler& errorHandler);
    template<typename TProtocol>
    std::shared_ptr<Connection<TProtocol>> makeConnection();

private:
    RequestProcessor requestProcessor_;
    sfun::member<IOService&> ioService_;
    sfun::member<ErrorHandler&> errorHandler_;
};
} // namespace asyncgi::detail
