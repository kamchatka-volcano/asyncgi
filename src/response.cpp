#include <asyncgi/response.h>
#include "requestcontext.h"

namespace asyncgi::detail{

Response::Response(std::shared_ptr<RequestContext> context, ITimer& timer, IClient& client)
        : context_(std::move(context))
        , timer_(timer)
        , client_(client)
{}

void Response::send(const http::Response& response)
{
    if (!context_->response().isValid())
        return;

    context_->response().setData(response.data());
    context_->response().send();
}

ITimer& Response::timer()
{
    return timer_;
}

IClient& Response::client()
{
    return client_;
}

bool Response::isSent() const
{
    return !context_->response().isValid();
}

}
