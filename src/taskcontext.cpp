#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/taskcontext.h>

namespace asyncgi {

TaskContext::PostAction::PostAction(std::function<void()> action)
    : action_{std::move(action)}
{
}

TaskContext::PostAction::~PostAction()
{
    if (action_)
        action_();
}

TaskContext::TaskContext(asio::io_context& io, std::function<void()> postTaskAction)
    : io_{io}
    , postTaskAction_{std::make_shared<TaskContext::PostAction>(std::move(postTaskAction))}
{
}

asio::io_context& TaskContext::io() const
{
    return io_;
}

} // namespace asyncgi
