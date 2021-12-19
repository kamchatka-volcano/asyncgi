#pragma once
#include "iserver.h"
#include "itimer.h"
#include "errors.h"
#include "types.h"
#include "requestprocessor.h"

namespace asyncgi{

class IApp{
public:
    virtual ~IApp() = default;
    virtual std::unique_ptr<IServer> makeServer(detail::IRequestProcessor&) = 0;
    virtual std::unique_ptr<IServer> makeServer(detail::IRequestProcessor&, ErrorHandlerFunc errorHandler) = 0;
    virtual std::unique_ptr<ITimer> makeTimer() = 0;
    virtual void exec() = 0;
};

}