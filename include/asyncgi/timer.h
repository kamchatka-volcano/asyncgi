#ifndef ASYNCGI_TIMER_H
#define ASYNCGI_TIMER_H

#include <chrono>
#include <functional>
#include <memory>

namespace asyncgi {

class IO;

namespace detail {
class TimerService;
}

class Timer {
public:
    Timer(IO&);
    ~Timer();
    Timer(const Timer&) = delete;
    Timer(Timer&&) = default;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = default;

    void start(std::chrono::milliseconds time, std::function<void()> callback);
    void startPeriodic(std::chrono::milliseconds time, std::function<void()> callback);
    void stop();

private:
    std::unique_ptr<detail::TimerService> timerService_;
};

} //namespace asyncgi

#endif //ASYNCGI_TIMER_H
