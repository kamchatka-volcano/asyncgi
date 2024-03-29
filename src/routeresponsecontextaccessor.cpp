#include "responsecontext.h"
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/detail/routeresponsecontextaccessor.h>
#include <asyncgi/responder.h>

namespace asyncgi::detail {

void RouterResponseContextAccessor::setRequestProcessorQueue(
        Responder& response,
        const std::shared_ptr<whaleroute::RequestProcessorQueue>& queue)
{
    if (auto context = response.context(sfun::access_token<RouterResponseContextAccessor>{}).lock())
        context->setRequestProcessorQueue(queue);
}

} //namespace asyncgi::detail