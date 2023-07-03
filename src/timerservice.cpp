#include "timerservice.h"
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/external/whaleroute/requestprocessorqueue.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/io_context.hpp>
#else
#include <asio/io_context.hpp>
#endif

namespace asyncgi::detail {

TimerService::TimerService(asio::io_context& io, whaleroute::RequestProcessorQueue* requestProcessorQueue)
    : io_{io}
    , timer_{io_.get()}
    , requestProcessorQueue_{requestProcessorQueue}
{
}

void TimerService::start(std::chrono::milliseconds time, std::function<void()> callback)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();
    timer_.expires_after(time);
    timer_.async_wait(
            [this, task = std::move(callback)](auto& ec) mutable
            {
                if (ec)
                    return;
                task();
                stop();
            });
}

void TimerService::startPeriodic(std::chrono::milliseconds time, std::function<void()> callback)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

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

void TimerService::stop()
{
    timer_.cancel();

    if (requestProcessorQueue_)
        requestProcessorQueue_->launch();
}

} // namespace asyncgi::detail
