#ifndef ASYNCGI_TIMER_H
#define ASYNCGI_TIMER_H

#include "detail/serviceholder.h"
#include "detail/utils.h"
#include <chrono>
#include <functional>
#include <future>
#include <memory>

namespace asyncgi {
class IO;
class Responder;

namespace detail {
class TimerService;
}

class Timer {
public:
    explicit Timer(IO&);
    explicit Timer(Responder&);

    void start(std::chrono::milliseconds time, std::function<void()> callback);
    void startPeriodic(std::chrono::milliseconds time, std::function<void()> callback);
    void stop();

    template<typename T, typename TCallable>
    void waitFuture(
            std::future<T>&& future,
            TCallable callback,
            [[maybe_unused]] std::chrono::milliseconds checkPeriod = std::chrono::milliseconds{1})
    {
        static_assert(std::is_invocable_v<TCallable, T>, "TCallable must be invokable with argument of type T");
        auto timerCallback = [fut = std::move(future), func = std::move(callback), stopTimer = stopSignal()]() mutable
        {
            if (fut.wait_for(std::chrono::seconds{0}) == std::future_status::ready) {
                func(fut.get());
                stopTimer();
            }
        };

        startPeriodic(checkPeriod, detail::makeCopyableLambda(std::move(timerCallback)));
    }

private:
    std::function<void()> stopSignal();

private:
    detail::ServiceHolder<detail::TimerService> timerService_;
};

} //namespace asyncgi

#endif //ASYNCGI_TIMER_H
