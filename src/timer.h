#pragma once
#include <asio/steady_timer.hpp>
#include <asyncgi/itimer.h>
#include <chrono>
#include <functional>
#include <future>
#include <map>

namespace asio {
class io_context;
}

namespace asyncgi::detail {

class Timer : public ITimer {
public:
    explicit Timer(asio::io_context& io);
    void start(std::chrono::milliseconds time, std::function<void()> callback) override;
    void start(std::chrono::milliseconds time, std::function<void()> callback, TimerMode mode) override;
    void stop() override;

private:
    asio::io_context& io_;
    asio::steady_timer timer_;
};

} // namespace asyncgi::detail
