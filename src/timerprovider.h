#pragma once
#include "timerservice.h"
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/detail/external/sfun/member.h>
#include <asyncgi/detail/external/sfun/optional_ref.h>
#include <deque>

namespace ASYNCGI_ASIO {
class io_context;
}

namespace asyncgi::whaleroute {
class RequestProcessorQueue;
}

namespace asyncgi::detail {

class TimerProvider {
public:
    explicit TimerProvider(asio::io_context& io);
    TimerService& emplaceTimer();
    void setRequestProcessorQueue(whaleroute::RequestProcessorQueue& queue);

private:
    sfun::member<asio::io_context&> io_;
    std::deque<TimerService> timers_;
    sfun::member<sfun::optional_ref<whaleroute::RequestProcessorQueue>> requestProcessorQueue_;
};

} // namespace asyncgi::detail
