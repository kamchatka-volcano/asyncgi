#pragma once
#include <hot_teacup/types.h>
#include <whaleroute/types.h>

namespace asyncgi{
using Access = whaleroute::RouteAccess;
using _ = whaleroute::_;

enum class FCGIParam{
    ContentType,
    RequestMethod,
    RemoteAddress,
    RequestURI,
    QueryString,
    HTTPCookie,
    HTTPHost
};

constexpr const char* fcgiParamStr(FCGIParam param)
{
    switch(param)
    {
        case FCGIParam::ContentType: return "CONTENT_TYPE";
        case FCGIParam::RequestMethod: return "REQUEST_METHOD";
        case FCGIParam::RemoteAddress: return "REMOTE_ADDR";
        case FCGIParam::RequestURI: return "REQUEST_URI";
        case FCGIParam::QueryString: return "QUERY_STRING";
        case FCGIParam::HTTPCookie: return "HTTP_COOKIE";
        case FCGIParam::HTTPHost: return "HTTP_HOST";
    }
    return nullptr;
}
}

