#include <asyncgi/timer.h>

namespace asyncgi{

Timer::Timer(asio::io_context& io)
    : io_(io)
{}

void Timer::stopTimer(unsigned timerId)
{
    auto& timer = timerStorage_.at(timerId);
    try{
        timer.cancel();
    } catch(const asio::system_error&){}
    timerStorage_.erase(timerId);
}

}


