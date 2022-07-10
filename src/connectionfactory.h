#pragma once
#include <asyncgi/errors.h>
#include <memory>

namespace asyncgi::detail{
template<typename TProtocol>
class Connection;
class IRuntime;
class IRequestProcessor;

class ConnectionFactory{
public:
    ConnectionFactory(IRequestProcessor& requestProcessor, IRuntime& runtime, ErrorHandlerFunc errorHandler);
    template<typename TProtocol>
    std::shared_ptr<Connection<TProtocol>> makeConnection();

private:
    IRequestProcessor& requestProcessor_;
    IRuntime& runtime_;
    ErrorHandlerFunc errorHandler_;
};
}
