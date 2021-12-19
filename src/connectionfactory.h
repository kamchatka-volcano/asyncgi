#pragma once
#include <asyncgi/errors.h>
#include <memory>

namespace asyncgi::detail{
class Connection;
class IRuntime;
class IRequestProcessor;

class ConnectionFactory{
public:
    ConnectionFactory(IRequestProcessor& requestProcessor, IRuntime& runtime, ErrorHandlerFunc errorHandler);
    std::shared_ptr<Connection> makeConnection();

private:
    IRequestProcessor& requestProcessor_;
    IRuntime& runtime_;
    ErrorHandlerFunc errorHandler_;
};
}
