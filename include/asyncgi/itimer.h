#pragma once
#include "detail/external/sfun/interface.h"
#include <chrono>
#include <functional>

namespace asyncgi {

enum class TimerMode {
    Once,
    Repeatedly
};

class ITimer : private sfun::Interface<ITimer> {
public:
    virtual void start(std::chrono::milliseconds, std::function<void()> callback) = 0;
    virtual void start(std::chrono::milliseconds, std::function<void()> callback, TimerMode) = 0;
    virtual void stop() = 0;
};

} // namespace asyncgi