#include <asyncgi/detail/responsecontext.h>
#include "timerprovider.h"

namespace asyncgi::detail{

ResponseContext::ResponseContext(
        IResponseSender& responseSender,
        TimerProvider& timerProvider,
        IClient& client,
        IAsioDispatcher& asioDispatcher)
    : responseSender_{responseSender}
    , timerProvider_{timerProvider}
    , client_{client}
    , asioDispatcher_{asioDispatcher}
{}

IResponseSender& ResponseContext::responseSender() const
{
    return responseSender_;
}

ITimer& ResponseContext::makeTimer() const
{
    return timerProvider_.get().emplaceTimer();
}

IClient& ResponseContext::client() const
{
    return client_;
}

IAsioDispatcher& ResponseContext::asioDispatcher() const
{
    return asioDispatcher_;
}

}
