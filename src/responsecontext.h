#pragma once
#include <asyncgi/detail/external/sfun/member.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace asyncgi::whaleroute {
class RequestProcessorQueue;
}

namespace asyncgi::detail {
class ResponseSender;
class TimerProvider;
class ClientService;
class AsioDispatcherService;

class ResponseContext {
public:
    ResponseContext(ResponseSender&, TimerProvider&, ClientService&, AsioDispatcherService&);

    ResponseSender& responseSender() const;
    ClientService& client() const;
    AsioDispatcherService& asioDispatcher() const;
    TimerProvider& timerProvider() const;

    void setRequestProcessorQueue(const std::shared_ptr<whaleroute::RequestProcessorQueue>& queue);

private:
    sfun::member<ResponseSender&> responseSender_;
    sfun::member<TimerProvider&> timerProvider_;
    sfun::member<ClientService&> client_;
    sfun::member<AsioDispatcherService&> asioDispatcher_;
    std::shared_ptr<whaleroute::RequestProcessorQueue> requestProcessorQueue_;
};

} // namespace asyncgi::detail