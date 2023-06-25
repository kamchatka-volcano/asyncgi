#include "asiodispatcherservice.h"
#include "clientservice.h"
#include "timerprovider.h"
#include <asyncgi/detail/iresponsesender.h>
#include <asyncgi/detail/responsecontext.h>

namespace asyncgi::detail {

ResponseContext::ResponseContext(
        IResponseSender& responseSender,
        TimerProvider& timerProvider,
        ClientService& client,
        AsioDispatcherService& asioDispatcher)
    : responseSender_{responseSender}
    , timerProvider_{timerProvider}
    , client_{client}
    , asioDispatcher_{asioDispatcher}
{
}

IResponseSender& ResponseContext::responseSender() const
{
    return responseSender_;
}

ITimerService& ResponseContext::makeTimer() const
{
    return timerProvider_.get().emplaceTimer();
}

ClientService& ResponseContext::client() const
{
    return client_;
}

AsioDispatcherService& ResponseContext::asioDispatcher() const
{
    return asioDispatcher_;
}

const std::vector<std::string>& ResponseContext::routeParams() const
{
    return routeParams_;
}

void ResponseContext::setRouteParams(const std::vector<std::string>& params)
{
    routeParams_ = params;
}

} // namespace asyncgi::detail
