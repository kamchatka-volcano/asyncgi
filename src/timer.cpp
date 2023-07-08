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

namespace {
detail::TimerService* getTimerService(Response& response, sfun::access_token<Timer> accessToken)
{
    if (auto context = response.context(accessToken).lock())
        return &context->timerProvider().emplaceTimer();
    else
        return nullptr;
}
} //namespace

Timer::Timer(Response& response)
    : timerService_{getTimerService(response, sfun::access_token<Timer>{})}
{
}

void Timer::start(std::chrono::milliseconds time, std::function<void()> callback)
{
    if (!timerService_.has_value())
        return;

    timerService_.get().start(time, callback);
}

void Timer::startPeriodic(std::chrono::milliseconds time, std::function<void()> callback)
{
    if (!timerService_.has_value())
        return;

    timerService_.get().startPeriodic(time, callback);
}

void Timer::stop()
{
    if (!timerService_.has_value())
        return;

    timerService_.get().stop();
}

std::function<void()> Timer::stopSignal()
{
    if (!timerService_.has_value())
        return [] {};

    return [timerService = &timerService_.get()]
    {
        timerService->stop();
    };
}

} //namespace asyncgi
