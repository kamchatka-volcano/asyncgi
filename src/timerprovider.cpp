#include "timerprovider.h"
#include "timerservice.h"
#include <asio/io_context.hpp>

namespace asyncgi::detail {

TimerProvider::TimerProvider(asio::io_context& io)
    : io_{io}
{
}

ITimerService& TimerProvider::emplaceTimer()
{
    return *timers_.emplace_back(std::make_unique<TimerService>(io_));
}

} // namespace asyncgi::detail
