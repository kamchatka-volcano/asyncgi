#include "ioservice.h"
#include "timerservice.h"
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/io.h>
#include <asyncgi/timer.h>

namespace asyncgi {

Timer::Timer(IO& io)
    : timerService_{std::make_unique<detail::TimerService>(io.ioService(sfun::access_token<Timer>{}).io())}
{
}

Timer::~Timer() = default;

void Timer::start(std::chrono::milliseconds time, std::function<void()> callback)
{
    timerService_->start(time, callback);
}

void Timer::startPeriodic(std::chrono::milliseconds time, std::function<void()> callback)
{
    timerService_->startPeriodic(time, callback);
}

void Timer::stop()
{
    timerService_->stop();
}

} //namespace asyncgi
