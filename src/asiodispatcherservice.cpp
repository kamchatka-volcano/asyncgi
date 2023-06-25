#include "asiodispatcherservice.h"
#include <asio/io_context.hpp>

namespace asyncgi::detail {

AsioDispatcherService::AsioDispatcherService(asio::io_context& io)
    : io_{io}
{
}

void AsioDispatcherService::postTask(
        std::function<void(const TaskContext& ctx)> task,
        std::function<void()> postTaskAction)
{
    auto taskContext = TaskContext{io_, std::move(postTaskAction)};
    io_.post(
            [task = std::move(task), taskContext = std::move(taskContext)]
            {
                task(taskContext);
            });
}

void AsioDispatcherService::postTask(std::function<void(const TaskContext& ctx)> task)
{
    postTask(std::move(task), {});
}

} // namespace asyncgi::detail