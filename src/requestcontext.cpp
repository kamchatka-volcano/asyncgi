#include "requestcontext.h"
#include <asyncgi/types.h>

namespace asyncgi::detail {

RequestContext::RequestContext(fcgi::Request&& request, fcgi::Response&& response)
    : response_(std::move(response))
    , fcgiRequest_(std::move(request))
    , request_(fcgiRequest_.param(fcgiParamStr(FCGIParam::RequestMethod)),
               fcgiRequest_.param(fcgiParamStr(FCGIParam::QueryString)),
               fcgiRequest_.param(fcgiParamStr(FCGIParam::HTTPCookie)),
               "Content-Type: " + fcgiRequest_.param(fcgiParamStr(FCGIParam::ContentType)),
               fcgiRequest_.stdIn())
{
}

const http::Request& RequestContext::request() const
{
    return request_;
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