#pragma once
#include "errors.h"
#include "iasiodispatcher.h"
#include "iclient.h"
#include "iserver.h"
#include "itimer.h"
#include "requestprocessor.h"
#include "types.h"
#include "detail/external/sfun/interface.h"

namespace asyncgi {

class IApp : private sfun::Interface<IApp> {
public:
    virtual std::unique_ptr<IServer> makeServer(RequestProcessor) const = 0;
    virtual std::unique_ptr<IServer> makeServer(RequestProcessor, ErrorHandlerFunc errorHandler) const = 0;
    virtual std::unique_ptr<ITimer> makeTimer() const = 0;
    virtual std::unique_ptr<IClient> makeClient() const = 0;
    virtual std::unique_ptr<IClient> makeClient(ErrorHandlerFunc errorHandler) const = 0;
    virtual std::unique_ptr<IAsioDispatcher> makeAsioDispatcher() const = 0;
    virtual void exec() = 0;
};

} // namespace asyncgi