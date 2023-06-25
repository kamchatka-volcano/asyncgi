#ifndef ASYNCGI_ITIMERSERVICE_H
#define ASYNCGI_ITIMERSERVICE_H

#include <asyncgi/detail/external/sfun/interface.h>
#include <chrono>
#include <functional>

namespace asyncgi::detail {

class ITimerService : private sfun::interface<ITimerService> {
public:
    virtual void start(std::chrono::milliseconds, std::function<void()> callback) = 0;
    virtual void startPeriodic(std::chrono::milliseconds, std::function<void()> callback) = 0;
    virtual void stop() = 0;
};

} //namespace asyncgi::detail

#endif //ASYNCGI_ITIMERSERVICE_H
