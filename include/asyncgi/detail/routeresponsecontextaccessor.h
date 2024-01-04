#ifndef ASYNCGI_ROUTERESPONSECONTEXTACCESSOR_H
#define ASYNCGI_ROUTERESPONSECONTEXTACCESSOR_H

#include <memory>

namespace asyncgi {
class Responder;

namespace whaleroute {
class RequestProcessorQueue;
}

namespace detail {

struct RouterResponseContextAccessor {
    static void setRequestProcessorQueue(Responder&, const std::shared_ptr<whaleroute::RequestProcessorQueue>&);
};

} //namespace detail

} //namespace asyncgi

#endif //ASYNCGI_ROUTERESPONSECONTEXTACCESSOR_H
