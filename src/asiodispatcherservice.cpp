#include "asiodispatcherservice.h"
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#else
#include <asio/io_context.hpp>
#include <asio/post.hpp>
#endif
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/external/whaleroute/requestprocessorqueue.h>

namespace asyncgi::detail {

AsioDispatcherService::AsioDispatcherService(asio::io_context& io)
    : io_{io}
{
}

AsioDispatcherService::~AsioDispatcherService()
{
    if (!hasTask_)
        return;

    if (auto queue = requestProcessorQueue_.lock())
        queue->launch();
}

void AsioDispatcherService::postTask(std::function<void(const AsioContext& ctx)> task)
{
    hasTask_ = true;
    if (auto queue = requestProcessorQueue_.lock())
        queue->stop();

    asio::post(
            io_.get(),
            [this, task = std::move(task)]
            {
                task(AsioContext{io_});
            });
}

void AsioDispatcherService::setRequestProcessorQueue(std::shared_ptr<whaleroute::RequestProcessorQueue> queue)
{
    requestProcessorQueue_ = queue;
}

} // namespace asyncgi::detail