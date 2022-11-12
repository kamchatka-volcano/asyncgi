#pragma once
#include <asyncgi/errors.h>
#include <asyncgi/requestprocessor.h>
#include <memory>

namespace asyncgi::detail{
template<typename TProtocol>
class Connection;
class IRuntime;

class ConnectionFactory{
public:
    ConnectionFactory(RequestProcessor requestProcessor, IRuntime& runtime, ErrorHandlerFunc errorHandler);
    template<typename TProtocol>
    std::shared_ptr<Connection<TProtocol>> makeConnection();

private:
    RequestProcessor requestProcessor_;
    IRuntime& runtime_;
    ErrorHandlerFunc errorHandler_;
};
}
