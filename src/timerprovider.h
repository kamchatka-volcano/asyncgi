#pragma once
#include "timerservice.h"
#include <asyncgi/detail/external/sfun/member.h>
#include <deque>

namespace asio {
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
    void setRequestProcessorQueue(whaleroute::RequestProcessorQueue* queue);

private:
    sfun::member<asio::io_context&> io_;
    std::deque<TimerService> timers_;
    whaleroute::RequestProcessorQueue* requestProcessorQueue_ = nullptr;
};

} // namespace asyncgi::detail
