#include <asyncgi/detail/requestproxy.h>
#include <asyncgi/http/request_view.h>
#include <asyncgi/types.h>
#include <fcgi_responder/request.h>

namespace asyncgi::detail {

RequestProxy::RequestProxy(const fcgi::Request& request)
    : fcgiRequest_{fastcgi::detail::RequestView{request}}
    , httpRequest_{[this]
                   {
                  return http::Request{http::RequestView{
                          fcgiRequest_.hasParam("REQUEST_METHOD") ? fcgiRequest_.param("REQUEST_METHOD")
                                                                  : std::string_view{},
                          fcgiRequest_.hasParam("REMOTE_ADDR") ? fcgiRequest_.param("REMOTE_ADDR") : std::string_view{},
                          fcgiRequest_.hasParam("HTTP_HOST") ? fcgiRequest_.param("HTTP_HOST") : std::string_view{},
                          fcgiRequest_.hasParam("REQUEST_URI") ? fcgiRequest_.param("REQUEST_URI") : std::string_view{},
                          fcgiRequest_.hasParam("QUERY_STRING") ? fcgiRequest_.param("QUERY_STRING")
                                                                : std::string_view{},
                          fcgiRequest_.hasParam("HTTP_COOKIE") ? fcgiRequest_.param("HTTP_COOKIE") : std::string_view{},
                          fcgiRequest_.hasParam("CONTENT_TYPE") ? fcgiRequest_.param("CONTENT_TYPE")
                                                                : std::string_view{},
                          fcgiRequest_.stdIn()}};
              }}
{
}

RequestProxy::operator const fastcgi::Request&() const
{
    return fcgiRequest_;
}

RequestProxy::operator const http::Request&() const
{
    return httpRequest_;
}

} // namespace asyncgi
