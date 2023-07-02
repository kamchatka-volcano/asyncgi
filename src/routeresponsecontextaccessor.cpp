#include "responsecontext.h"
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/detail/routeresponsecontextaccessor.h>
#include <asyncgi/response.h>

namespace asyncgi::detail {

void RouterResponseContextAccessor::setRequestProcessorQueue(
        Response& response,
        std::shared_ptr<whaleroute::RequestProcessorQueue> queue)
{
    response.context(sfun::access_token<RouterResponseContextAccessor>{}).setRequestProcessorQueue(queue);
}

} //namespace asyncgi::detail