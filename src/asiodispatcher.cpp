#include "asiodispatcherservice.h"
#include "ioservice.h"
#include "responsecontext.h"
#include <asyncgi/asiodispatcher.h>
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/io.h>
#include <asyncgi/response.h>

namespace asyncgi {

AsioDispatcher::AsioDispatcher(IO& io)
    : asioDispatcherService_{
              std::make_unique<detail::AsioDispatcherService>(io.ioService(sfun::access_token<AsioDispatcher>{}).io())}
{
}

AsioDispatcher::AsioDispatcher(Response& response)
    : asioDispatcherService_{response.context(sfun::access_token<AsioDispatcher>{}).asioDispatcher()}
{
}

void AsioDispatcher::postTask(std::function<void(const TaskContext&)> task)
{
    asioDispatcherService_.get().postTask(std::move(task));
}

} //namespace asyncgi