#pragma once
#include <hot_teacup/types.h>
#include <whaleroute/types.h>
#include <map>
#include <string>

namespace asyncgi{
using Access = whaleroute::RouteAccess;
using _ = whaleroute::_;

struct FCGIRequest{
    std::map<std::string, std::string> params;
    std::string stdIn;
};

struct FCGIResponse{
    std::string data;
    std::string errorMessage;
};

namespace detail {
struct EmptyContext {
};
}

}

