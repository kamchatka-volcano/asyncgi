#include "timerprovider.h"
#include "timerservice.h"
#include <asio/io_context.hpp>

namespace asyncgi::detail {

TimerProvider::TimerProvider(asio::io_context& io)
    : io_{io}
{
}

TimerService& TimerProvider::emplaceTimer()
{
    return timers_.emplace_back(io_, requestProcessorQueue_);
}

void TimerProvider::setRequestProcessorQueue(whaleroute::RequestProcessorQueue* queue)
{
    requestProcessorQueue_ = queue;
}

} // namespace asyncgi::detail
