#pragma once
#include <asyncgi/taskcontext.h>

namespace asio {
class io_context;
}

namespace asyncgi::detail {

class AsioDispatcherService {
public:
    explicit AsioDispatcherService(asio::io_context& io);
    void postTask(std::function<void(const TaskContext&)> task, std::function<void()> postTaskAction);
    void postTask(std::function<void(const TaskContext&)> task);

private:
    asio::io_context& io_;
};

} // namespace asyncgi::detail
