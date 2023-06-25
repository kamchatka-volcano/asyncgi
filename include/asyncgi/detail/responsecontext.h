#pragma once
#include <asyncgi/detail/external/sfun/member.h>
#include <functional>
#include <string>
#include <vector>

namespace asyncgi::detail {
class IResponseSender;
class TimerProvider;
class ITimerService;
class ClientService;
class AsioDispatcherService;

class ResponseContext {
public:
    ResponseContext(IResponseSender&, TimerProvider&, ClientService&, AsioDispatcherService&);

    IResponseSender& responseSender() const;
    ITimerService& makeTimer() const;
    ClientService& client() const;
    AsioDispatcherService& asioDispatcher() const;
    const std::vector<std::string>& routeParams() const;

    void setRouteParams(const std::vector<std::string>& params);

private:
    sfun::member<IResponseSender&> responseSender_;
    sfun::member<TimerProvider&> timerProvider_;
    sfun::member<ClientService&> client_;
    sfun::member<AsioDispatcherService&> asioDispatcher_;
    std::vector<std::string> routeParams_;
};

} // namespace asyncgi::detail