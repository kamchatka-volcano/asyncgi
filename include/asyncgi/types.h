#pragma once
#include <hot_teacup/types.h>
#include <whaleroute/types.h>
#include <map>
#include <string>

namespace asyncgi{
using _ = whaleroute::_;

namespace detail {
struct EmptyRouteContext {
};
}
}

namespace fastcgi{
    struct Response{
        std::string data;
        std::string errorMsg;
    };

    struct Request{
        std::map<std::string, std::string> params;
        std::string stdIn;
    };
}

