#include "asiodispatcherservice.h"
#include <asio/io_context.hpp>
#include <asyncgi/detail/external/whaleroute/requestprocessorqueue.h>

namespace asyncgi::detail {

AsioDispatcherService::AsioDispatcherService(asio::io_context& io)
    : io_{io}
{
}

void AsioDispatcherService::postTask(std::function<void(const TaskContext& ctx)> task)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

    auto postTaskAction = [this]
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->launch();
    };
    auto taskContext = TaskContext{io_, std::move(postTaskAction)};
    io_.get().post(
            [task = std::move(task), taskContext = std::move(taskContext)]
            {
                task(taskContext);
            });
}

void AsioDispatcherService::setRequestProcessorQueue(whaleroute::RequestProcessorQueue* queue)
{
    requestProcessorQueue_ = queue;
}

} // namespace asyncgi::detail