#ifndef ASYNCGI_TYPES_H
#define ASYNCGI_TYPES_H

#include "detail/external/whaleroute/types.h"
#include "http/types.h"
#include <map>
#include <string>

namespace asyncgi {
using _ = whaleroute::_;

using rx = whaleroute::rx;
namespace string_literals = whaleroute::string_literals;
using TrailingSlashMode = whaleroute::TrailingSlashMode;
template<int minSize = 0>
using RouteParameters = whaleroute::RouteParameters<minSize>;
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

#endif //ASYNCGI_TYPES_H