#pragma once
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/external/sfun/member.h>
#include <asyncgi/taskcontext.h>

namespace ASYNCGI_ASIO {
class io_context;
}

namespace asyncgi::whaleroute {
class RequestProcessorQueue;
}

namespace asyncgi::detail {

class AsioDispatcherService {
public:
    explicit AsioDispatcherService(asio::io_context& io);
    void postTask(std::function<void(const TaskContext&)> task);
    void setRequestProcessorQueue(whaleroute::RequestProcessorQueue* queue);

private:
    sfun::member<asio::io_context&> io_;
    whaleroute::RequestProcessorQueue* requestProcessorQueue_ = nullptr;
};

} // namespace asyncgi::detail
