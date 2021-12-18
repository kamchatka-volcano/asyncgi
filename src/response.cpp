#include <asyncgi/response.h>
#include "requestcontext.h"

namespace asyncgi{

Response::Response(std::shared_ptr<RequestContext> context, Timer& timer)
        : context_(std::move(context)), timer_(timer)
{}

void Response::send(const http::Response& response)
{
    if (!context_->response().isValid())
        return;

    context_->response().setData(response.data());
    context_->response().send();
}

Timer& Response::timer()
{
    return timer_.get();
}

bool Response::isSent() const
{
    return !context_->response().isValid();
}

}
