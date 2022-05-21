#pragma once
#include "iserver.h"
#include "itimer.h"
#include "iclient.h"
#include "errors.h"
#include "types.h"
#include "requestprocessor.h"

namespace asyncgi{

class IApp{
public:
    virtual ~IApp() = default;
    virtual std::unique_ptr<IServer> makeServer(detail::IRequestProcessor&) const = 0;
    virtual std::unique_ptr<IServer> makeServer(detail::IRequestProcessor&, ErrorHandlerFunc errorHandler) const = 0;
    virtual std::unique_ptr<ITimer> makeTimer() const = 0;
    virtual std::unique_ptr<IClient> makeClient(ErrorHandlerFunc errorHandler) const = 0;
    virtual void exec() = 0;
};

}