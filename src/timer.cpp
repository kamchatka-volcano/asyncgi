#include "timer.h"
#include <asio/io_context.hpp>

namespace asyncgi::detail {

Timer::Timer(asio::io_context& io)
    : io_{io}
    , timer_{io_}
{
}

void Timer::start(std::chrono::milliseconds time, std::function<void()> callback)
{
    timer_.expires_after(time);
    timer_.async_wait(
            [this, task = std::move(callback)](auto& ec) mutable
            {
                if (ec)
                    return;
                stop();
                task();
            });
}

void Timer::startPeriodic(std::chrono::milliseconds time, std::function<void()> callback)
{
    timer_.expires_after(time);
    timer_.async_wait(
            [this, time, task = std::move(callback)](auto& ec) mutable
            {
                if (ec)
                    return;
                task();
                startPeriodic(time, std::move(task));
            });
}

void Timer::stop()
{
    timer_.cancel();
}

} // namespace asyncgi::detail
