#include "ioservice.h"
#include "responsecontext.h"
#include "timerprovider.h"
#include "timerservice.h"
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/io.h>
#include <asyncgi/response.h>
#include <asyncgi/timer.h>

namespace asyncgi {

Timer::Timer(IO& io)
    : timerService_{std::make_unique<detail::TimerService>(io.ioService(sfun::access_token<Timer>{}).io())}
{
}

Timer::Timer(Response& response)
    : timerService_{response.context(sfun::access_token<Timer>{}).timerProvider().emplaceTimer()}
{
}

void Timer::start(std::chrono::milliseconds time, std::function<void()> callback)
{
    timerService_.get().start(time, callback);
}

void Timer::startPeriodic(std::chrono::milliseconds time, std::function<void()> callback)
{
    timerService_.get().startPeriodic(time, callback);
}

void Timer::stop()
{
    timerService_.get().stop();
}

std::function<void()> Timer::stopSignal()
{
    return [timerService = &timerService_.get()]
    {
        timerService->stop();
    };
}

} //namespace asyncgi
