#pragma once
#include <chrono>
#include <functional>

namespace asyncgi{

enum class TimerMode{
    Once,
    Repeatedly
};

class ITimer
{
public:
    virtual ~ITimer() = default;
    virtual void start(std::chrono::milliseconds, std::function<void()> callback) = 0;
    virtual void start(std::chrono::milliseconds, std::function<void()> callback, TimerMode) = 0;
    virtual void stop() = 0;
};

}