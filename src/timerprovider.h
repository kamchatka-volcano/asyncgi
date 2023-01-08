#pragma once
#include <asyncgi/itimer.h>
#include <memory>
#include <vector>

namespace asio {
class io_context;
}

namespace asyncgi::detail {

class TimerProvider {
public:
    explicit TimerProvider(asio::io_context& io);
    ITimer& emplaceTimer();

private:
    asio::io_context& io_;
    std::vector<std::unique_ptr<ITimer>> timers_;
};

} // namespace asyncgi::detail
