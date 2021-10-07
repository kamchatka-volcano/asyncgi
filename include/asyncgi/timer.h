#pragma once
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <map>
#include <future>

namespace asyncgi{

enum class TimerMode{
    Once,
    Repeatedly
};

class Timer{
public:
    Timer(asio::io_context&);

    template <typename TDuration, typename TCallable>
    unsigned startTimer(TDuration time, TCallable task, TimerMode mode = TimerMode::Repeatedly);

    template <typename TDuration, typename TCallable>
    void resetTimer(unsigned timerId, TDuration time, TCallable task, TimerMode mode = TimerMode::Repeatedly);

    void stopTimer(unsigned timerId);

    template <typename T, typename TCallable, typename TDuration = asio::chrono::microseconds>
    unsigned waitFuture(std::future<T>&& future, TCallable callback, TDuration checkPeriod = asio::chrono::microseconds{250});

private:
    template <typename T, typename TCallable, typename TDuration = asio::chrono::microseconds>
    void resetFutureTimer(unsigned timerId, std::future<T>&& future, TCallable callback, TDuration checkPeriod);

private:
    asio::io_context& io_;
    std::map<unsigned, asio::steady_timer> timerStorage_;
    unsigned idGenerator_ = 0;
};


template <typename TDuration, typename TCallable>
unsigned Timer::startTimer(TDuration time, TCallable task, TimerMode mode)
{
    auto timerId = ++idGenerator_;
    resetTimer(timerId, time, task, mode);
    return timerId;
}

template <typename TDuration, typename TCallable>
void Timer::resetTimer(unsigned timerId, TDuration time, TCallable task, TimerMode mode)
{
    auto& timer = timerStorage_.emplace(timerId, io_).first->second;
    timer.expires_after(time);
    timer.async_wait(
        [this, timerId, time, task, mode](auto& ec) mutable{
            if (ec)
                return;
            task(timerId);
            if (mode == TimerMode::Repeatedly)
                resetTimer(timerId, time, task);
            else
                stopTimer(timerId);
        });
}

template <typename T, typename TCallable, typename TDuration>
unsigned Timer::waitFuture(std::future<T>&& future, TCallable callback, TDuration checkPeriod)
{
    static_assert (std::is_invocable_v<TCallable, T>, "TCallable must be invokable with argument of type T");
    if (!future.valid())
        return 0;
    auto timerId = ++idGenerator_;
    resetFutureTimer(timerId, std::move(future), callback, checkPeriod);
    return timerId;
}

template <typename T, typename TCallable, typename TDuration>
void Timer::resetFutureTimer(unsigned timerId, std::future<T>&& future, TCallable callback, TDuration checkPeriod)
{
    auto& timer = timerStorage_.emplace(timerId, io_).first->second;
    timer.expires_after(checkPeriod);
    timer.async_wait(
        [this, fut = std::move(future), timerId, checkPeriod, callback](auto& ec) mutable{
            if (ec)
                return;
            if (fut.wait_for(std::chrono::seconds{0}) == std::future_status::ready){
                callback(fut.get());
                stopTimer(timerId);
            }
            else
                resetFutureTimer(timerId, std::move(fut), callback, checkPeriod);
        });
}


}

