#pragma once
#include <functional>

namespace asyncgi{
class ITimer;
class IClient;
class IAsioDispatcher;
}

namespace asyncgi::detail {
class IResponseSender;
class TimerProvider;

class ResponseContext{
public:
    ResponseContext(IResponseSender&, TimerProvider&, IClient&, IAsioDispatcher&);

    IResponseSender& responseSender() const;
    ITimer& makeTimer() const;
    IClient& client() const;
    IAsioDispatcher& asioDispatcher() const;

private:
    std::reference_wrapper<IResponseSender> responseSender_;
    std::reference_wrapper<TimerProvider> timerProvider_;
    std::reference_wrapper<IClient> client_;
    std::reference_wrapper<IAsioDispatcher> asioDispatcher_;
};

}