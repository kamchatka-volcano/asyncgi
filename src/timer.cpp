#include "timer.h"
#include <asio/io_context.hpp>

namespace asyncgi::detail{

Timer::Timer(asio::io_context& io)
    : io_{io}
    , timer_{io_}
{}

void Timer::start(std::chrono::milliseconds time, std::function<void()> callback, TimerMode mode)
{
    timer_.expires_after(time);
    timer_.async_wait(
            [this, time, task = std::move(callback), mode](auto& ec) mutable {
                if (ec)
                    return;

                if (mode == TimerMode::Repeatedly) {
                    task();
                    start(time, std::move(task), mode);
                }
                else {
                    stop();
                    task();
                }
            });
}

void Timer::start(std::chrono::milliseconds time, std::function<void()> callback)
{
    start(time, std::move(callback), TimerMode::Repeatedly);
}

void Timer::stop()
{
    timer_.cancel();
}

}


