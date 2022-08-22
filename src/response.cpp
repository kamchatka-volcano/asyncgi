#include <asyncgi/response.h>
#include "timerprovider.h"
#include <fcgi_responder/response.h>

namespace asyncgi::detail{

Response::Response(
        fcgi::Response& fcgiResponse,
        TimerProvider& timerProvider,
        IClient& client,
        IAsioDispatcher& asioDispatcher)
    : fcgiResponse_{fcgiResponse}
    , timerProvider_{timerProvider}
    , client_{client}
    , asioDispatcher_{asioDispatcher}
{}

void Response::send(const http::Response& response)
{
    if (!fcgiResponse().isValid())
        return;

    fcgiResponse().setData(response.data());
    fcgiResponse().send();
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
    return !fcgiResponse().isValid();
}

}
