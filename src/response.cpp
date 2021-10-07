#include <asyncgi/response.h>
#include "requestcontext.h"

namespace asyncgi{

Response::Response(std::shared_ptr<RequestContext> context, Timer& timer)
    : context_(std::move(context))
    , timer_(timer)
{}

void Response::send()
{
    if (!context_->response() && value_)
        return;

    context_->response().setData(value_->data());
    context_->response().send();
}

Timer& Response::timer()
{
    return timer_.get();
}

}
