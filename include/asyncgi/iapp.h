#pragma once
#include "iserver.h"
#include "itimer.h"
#include "iclient.h"
#include "iasiodispatcher.h"
#include "errors.h"
#include "types.h"
#include "requestprocessor.h"

namespace asyncgi{

class IApp{
public:
    IApp() = default;
    virtual ~IApp() = default;
    IApp(const IApp&) = delete;
    IApp& operator=(const IApp&) = delete;
    IApp(IApp&&) = delete;
    IApp&& operator=(IApp&&) = delete;

    virtual std::unique_ptr<IServer> makeServer(detail::IRequestProcessor&) const = 0;
    virtual std::unique_ptr<IServer> makeServer(detail::IRequestProcessor&, ErrorHandlerFunc errorHandler) const = 0;
    virtual std::unique_ptr<ITimer> makeTimer() const = 0;
    virtual std::unique_ptr<IClient> makeClient() const = 0;
    virtual std::unique_ptr<IClient> makeClient(ErrorHandlerFunc errorHandler) const = 0;
    virtual std::unique_ptr<IAsioDispatcher> makeAsioDispatcher() const = 0;
    virtual void exec() = 0;
};

}