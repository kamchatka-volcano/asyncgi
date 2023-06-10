#pragma once
#include "detail/external/sfun/interface.h"
#include <chrono>
#include <functional>

namespace asyncgi {

class ITimer : private sfun::Interface<ITimer> {
public:
    virtual void start(std::chrono::milliseconds, std::function<void()> callback) = 0;
    virtual void startPeriodic(std::chrono::milliseconds, std::function<void()> callback) = 0;
    virtual void stop() = 0;
};

} // namespace asyncgi