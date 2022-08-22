#include <asyncgi/response.h>
#include "requestcontext.h"
#include "timerprovider.h"

namespace asyncgi::detail{

Response::Response(
        std::shared_ptr<RequestContext> context,
        TimerProvider& timerProvider,
        IClient& client,
        IAsioDispatcher& asioDispatcher)
    : context_{std::move(context)}
    , timerProvider_{timerProvider}
    , client_{client}
    , asioDispatcher_{asioDispatcher}
{}

void Response::send(const http::Response& response)
{
    if (!context_->response().isValid())
        return;

    context_->response().setData(response.data());
    context_->response().send();
}

ITimer& Response::makeTimer()
{
    return timerProvider_.get().emplaceTimer();
}

IClient& Response::client()
{
    return client_;
}

IAsioDispatcher& Response::asioDispatcher()
{
    return asioDispatcher_;
}

bool Response::isSent() const
{
    return !context_->response().isValid();
}

}
