#include "timerprovider.h"
#include "timer.h"
#include <asio/io_context.hpp>

namespace asyncgi::detail {

TimerProvider::TimerProvider(asio::io_context& io)
    : io_{io}
{
}

ITimer& TimerProvider::emplaceTimer()
{
    return *timers_.emplace_back(std::make_unique<Timer>(io_));
}

} // namespace asyncgi::detail
