#pragma once
#include "http/types.h"
#include <whaleroute/types.h>
#include <map>
#include <string>

namespace asyncgi {
using _ = whaleroute::_;

using rx = whaleroute::rx;
namespace string_literals = whaleroute::string_literals;
using RegexMode = whaleroute::RegexMode;
using TrailingSlashMode = whaleroute::TrailingSlashMode;
} // namespace asyncgi

namespace fastcgi {
struct Response {
    std::string data;
    std::string errorMsg;
};

struct Request {
    std::map<std::string, std::string> params;
    std::string stdIn;
};
} // namespace fastcgi
