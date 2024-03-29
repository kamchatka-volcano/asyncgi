#include "asiodispatcherservice.h"
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/io_context.hpp>
#else
#include <asio/io_context.hpp>
#endif
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/external/whaleroute/requestprocessorqueue.h>

namespace asyncgi::detail {

AsioDispatcherService::AsioDispatcherService(asio::io_context& io)
    : io_{io}
{
}

void AsioDispatcherService::postTask(std::function<void(const TaskContext& ctx)> task)
{
    if (auto queue = requestProcessorQueue_.lock())
        queue->stop();

    auto postTaskAction = [queueObserver = requestProcessorQueue_]
    {
        if (auto queue = queueObserver.lock())
            queue->launch();
    };

    auto taskContext = TaskContext{io_, std::move(postTaskAction)};
    io_.get().post(
            [task = std::move(task), taskContext = std::move(taskContext)]
            {
                task(taskContext);
            });
}

void AsioDispatcherService::setRequestProcessorQueue(std::shared_ptr<whaleroute::RequestProcessorQueue> queue)
{
    requestProcessorQueue_ = queue;
}

} // namespace asyncgi::detail