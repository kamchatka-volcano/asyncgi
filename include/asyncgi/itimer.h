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
    ITimer() = default;
    virtual ~ITimer() = default;
    ITimer(const ITimer&) = delete;
    ITimer& operator=(const ITimer&) = delete;
    ITimer(ITimer&&) = delete;
    ITimer&& operator=(ITimer&&) = delete;

public:
    virtual void start(std::chrono::milliseconds, std::function<void()> callback) = 0;
    virtual void start(std::chrono::milliseconds, std::function<void()> callback, TimerMode) = 0;
    virtual void stop() = 0;
};

}