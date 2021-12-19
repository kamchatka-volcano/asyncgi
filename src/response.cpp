#include <asyncgi/response.h>
#include "requestcontext.h"

namespace asyncgi::detail{

Response::Response(std::shared_ptr<RequestContext> context, ITimer& timer)
        : context_(std::move(context)), timer_(timer)
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
    return timer_.get();
}

bool Response::isSent() const
{
    return !context_->response().isValid();
}

}
