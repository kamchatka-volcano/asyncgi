#pragma once
#include <asyncgi/errors.h>
#include <memory>

namespace asyncgi{
class Connection;
class IRuntime;
class RequestProcessor;

class ConnectionFactory{
public:
    ConnectionFactory(RequestProcessor& requestProcessor, IRuntime& runtime, ErrorHandlerFunc errorHandler);
    std::shared_ptr<Connection> makeConnection();

private:
    RequestProcessor& requestProcessor_;
    IRuntime& runtime_;
    ErrorHandlerFunc errorHandler_;
};
}
