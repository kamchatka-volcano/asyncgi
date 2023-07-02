#pragma once
#include <asio/steady_timer.hpp>
#include <asyncgi/detail/external/sfun/member.h>
#include <chrono>
#include <functional>
#include <future>
#include <map>

namespace asio {
class io_context;
}

namespace asyncgi::whaleroute {
class RequestProcessorQueue;
}

namespace asyncgi::detail {

class TimerService {
public:
    explicit TimerService(asio::io_context& io, whaleroute::RequestProcessorQueue* requestProcessorQueue_ = nullptr);
    void start(std::chrono::milliseconds time, std::function<void()> callback);
    void startPeriodic(std::chrono::milliseconds time, std::function<void()> callback);
    void stop();

private:
    sfun::member<asio::io_context&> io_;
    asio::steady_timer timer_;
    whaleroute::RequestProcessorQueue* requestProcessorQueue_ = nullptr;
};

} // namespace asyncgi::detail
