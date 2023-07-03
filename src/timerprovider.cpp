#include "timerprovider.h"
#include "timerservice.h"
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/io_context.hpp>
#else
#include <asio/io_context.hpp>
#endif

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
