#include "requestcontext.h"
#include <asyncgi/types.h>

namespace asyncgi::detail {

RequestContext::RequestContext(fcgi::Request&& request, fcgi::Response&& response)
    : response_(std::move(response))
    , fcgiRequest_(std::move(request))
{
}

const http::Request& RequestContext::request()
{
    if (!request_)
        request_.emplace(fcgiRequest_.paramMap(), fcgiRequest_.stdIn());

    return *request_;
}

fcgi::Response& RequestContext::response()
{
    return response_;
}

const fcgi::Request& RequestContext::fcgiRequest() const
{
    return fcgiRequest_;
}

}