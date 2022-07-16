#include "requestcontext.h"
#include <asyncgi/types.h>

namespace asyncgi::detail {

RequestContext::RequestContext(fcgi::Request&& request, fcgi::Response&& response)
    : response_{std::move(response)}
    , fcgiRequest_{std::move(request)}
{
}

const http::RequestView& RequestContext::request()
{
    if (!request_)
        request_.emplace(
                fcgiRequest_.hasParam("REQUEST_METHOD") ? fcgiRequest_.param("REQUEST_METHOD") : "",
                fcgiRequest_.hasParam("REMOTE_ADDR") ? fcgiRequest_.param("REMOTE_ADDR") : "",
                fcgiRequest_.hasParam("HTTP_HOST") ? fcgiRequest_.param("HTTP_HOST") : "",
                fcgiRequest_.hasParam("REQUEST_URI") ? fcgiRequest_.param("REQUEST_URI") : "",
                fcgiRequest_.hasParam("QUERY_STRING") ? fcgiRequest_.param("QUERY_STRING") : "",
                fcgiRequest_.hasParam("HTTP_COOKIE") ? fcgiRequest_.param("HTTP_COOKIE") : "",
                fcgiRequest_.hasParam("CONTENT_TYPE") ? fcgiRequest_.param("CONTENT_TYPE") : "",
                fcgiRequest_.stdIn());

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