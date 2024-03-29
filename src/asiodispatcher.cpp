#include "asiodispatcherservice.h"
#include "ioservice.h"
#include "responsecontext.h"
#include <asyncgi/asiodispatcher.h>
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/detail/external/sfun/optional_ref.h>
#include <asyncgi/io.h>
#include <asyncgi/responder.h>

namespace asyncgi {

AsioDispatcher::AsioDispatcher(IO& io)
    : asioDispatcherService_{
              std::make_unique<detail::AsioDispatcherService>(io.ioService(sfun::access_token<AsioDispatcher>{}).io())}
{
}

namespace {
sfun::optional_ref<detail::AsioDispatcherService> getAsioDispatcherService(
        Responder& response,
        sfun::access_token<AsioDispatcher> accessToken)
{
    if (auto context = response.context(accessToken).lock())
        return context->asioDispatcher();
    else
        return std::nullopt;
}
} //namespace

AsioDispatcher::AsioDispatcher(Responder& response)
    : asioDispatcherService_{getAsioDispatcherService(response, sfun::access_token<AsioDispatcher>{})}
{
}

void AsioDispatcher::postTask(std::function<void(const TaskContext&)> task)
{
    if (!asioDispatcherService_.has_value())
        return;
    asioDispatcherService_.get().postTask(std::move(task));
}

} //namespace asyncgi