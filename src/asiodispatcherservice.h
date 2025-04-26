#pragma once
#include <asyncgi/asio_context.h>
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/external/sfun/member.h>
#include <functional>
#include <memory>

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
    ~AsioDispatcherService();
    AsioDispatcherService(const AsioDispatcherService&) = default;
    AsioDispatcherService(AsioDispatcherService&&) = default;
    AsioDispatcherService& operator=(const AsioDispatcherService&) = default;
    AsioDispatcherService& operator=(AsioDispatcherService&&) = default;

    void postTask(std::function<void(const AsioContext&)> task);
    void setRequestProcessorQueue(std::shared_ptr<whaleroute::RequestProcessorQueue> queue);

private:
    sfun::member<asio::io_context&> io_;
    std::weak_ptr<whaleroute::RequestProcessorQueue> requestProcessorQueue_;
    bool hasTask_ = false;
};

} // namespace asyncgi::detail
