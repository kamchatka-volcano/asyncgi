#include "responsecontext.h"
#include "asiodispatcherservice.h"
#include "clientservice.h"
#include "timerprovider.h"

namespace asyncgi::detail {

ResponseContext::ResponseContext(
        ResponseSender& responseSender,
        TimerProvider& timerProvider,
        ClientService& client,
        AsioDispatcherService& asioDispatcher)
    : responseSender_{responseSender}
    , timerProvider_{timerProvider}
    , client_{client}
    , asioDispatcher_{asioDispatcher}
{
}

ResponseSender& ResponseContext::responseSender() const
{
    return responseSender_;
}

ClientService& ResponseContext::client() const
{
    return client_;
}

AsioDispatcherService& ResponseContext::asioDispatcher() const
{
    return asioDispatcher_;
}

TimerProvider& ResponseContext::timerProvider() const
{
    return timerProvider_;
}

void ResponseContext::setRequestProcessorQueue(const std::shared_ptr<whaleroute::RequestProcessorQueue>& queue)
{
    requestProcessorQueue_ = queue;
    client_.get().setRequestProcessorQueue(requestProcessorQueue_.get());
    asioDispatcher_.get().setRequestProcessorQueue(requestProcessorQueue_.get());
    timerProvider_.get().setRequestProcessorQueue(requestProcessorQueue_.get());
}

} // namespace asyncgi::detail
