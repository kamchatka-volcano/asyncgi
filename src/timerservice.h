#pragma once
#include "asyncgi/detail/itimerservice.h"
#include <asio/steady_timer.hpp>
#include <chrono>
#include <functional>
#include <future>
#include <map>

namespace asio {
class io_context;
}

namespace asyncgi::detail {

class IOService;

class TimerService : public ITimerService {
public:
    explicit TimerService(asio::io_context& io);
    void start(std::chrono::milliseconds time, std::function<void()> callback) override;
    void startPeriodic(std::chrono::milliseconds time, std::function<void()> callback) override;
    void stop() override;

private:
    asio::io_context& io_;
    asio::steady_timer timer_;
};

} // namespace asyncgi::detail
