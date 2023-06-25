#include "asiodispatcherservice.h"
#include "ioservice.h"
#include <asyncgi/asiodispatcher.h>
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/io.h>

namespace asyncgi {

AsioDispatcher::AsioDispatcher(IO& io)
    : asioDispatcherService_{
              std::make_unique<detail::AsioDispatcherService>(io.ioService(sfun::access_token<AsioDispatcher>{}).io())}
{
}

AsioDispatcher::~AsioDispatcher() = default;

void AsioDispatcher::postTask(std::function<void(const TaskContext&)> task, std::function<void()> postTaskAction)
{
    asioDispatcherService_->postTask(std::move(task), std::move(postTaskAction));
}
void AsioDispatcher::postTask(std::function<void(const TaskContext&)> task)
{
    asioDispatcherService_->postTask(std::move(task));
}

} //namespace asyncgi