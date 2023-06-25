#pragma once
#include <asyncgi/detail/itimerservice.h>
#include <memory>
#include <vector>

namespace asio {
class io_context;
}

namespace asyncgi::detail {

class ITimerService;

class TimerProvider {
public:
    explicit TimerProvider(asio::io_context& io);
    ITimerService& emplaceTimer();

private:
    asio::io_context& io_;
    std::vector<std::unique_ptr<ITimerService>> timers_;
};

} // namespace asyncgi::detail
