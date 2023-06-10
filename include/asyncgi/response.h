#pragma once
#include "iasiodispatcher.h"
#include "iclient.h"
#include "itimer.h"
#include "types.h"
#include "detail/external/whaleroute/requestprocessorqueue.h"
#include "detail/iresponsesender.h"
#include "detail/responsecontext.h"
#include "http/request.h"
#include "http/response.h"
#include <functional>
#include <future>
#include <memory>
#include <optional>

namespace asyncgi {
template<typename TContext>
class RequestRouter;

namespace detail {

template<class F>
auto make_copyable_function(F&& f)
{
    using dF = std::decay_t<F>;
    auto spf = std::make_shared<dF>(std::forward<F>(f));
    return [spf](auto&&... args) -> decltype(auto)
    {
        return (*spf)(decltype(args)(args)...);
    };
}
} // namespace detail

class Response {
public:
    explicit Response(const detail::ResponseContext& responseContext);

    template<typename... TArgs>
    void send(TArgs... args)
    {
        auto response = http::Response{std::forward<TArgs>(args)...};
        responseContext_.responseSender().send(response.data(http::ResponseMode::Cgi));
    }

    template<typename T, typename TCallable>
    void waitFuture(
            std::future<T>&& future,
            TCallable callback,
            std::chrono::milliseconds checkPeriod = std::chrono::milliseconds{1})
    {
        static_assert(std::is_invocable_v<TCallable, T>, "TCallable must be invokable with argument of type T");
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();
        auto& timer = responseContext_.makeTimer();
        auto timerCallback = [fut = std::move(future), func = std::move(callback), &timer, checkPeriod, this]() mutable
        {
            if (fut.wait_for(std::chrono::seconds{0}) == std::future_status::ready) {
                timer.stop();
                func(fut.get());
                if (requestProcessorQueue_)
                    requestProcessorQueue_->launch();
            }
            else
                waitFuture(std::move(fut), std::move(func), checkPeriod);
        };
        timer.start(checkPeriod, detail::make_copyable_function(std::move(timerCallback)));
    }

    void redirect(
            std::string path,
            http::RedirectType redirectType = http::RedirectType::Found,
            std::vector<http::Cookie> cookies = {},
            std::vector<http::Header> headers = {});

    void send(const http::Response& response);
    void send(fastcgi::Response response);
    bool isSent() const;
    void executeTask(std::function<void(const TaskContext& ctx)> task);

    void makeRequest(
            const std::filesystem::path& socketPath,
            fastcgi::Request request,
            const std::function<void(std::optional<fastcgi::Response>)>& responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void makeRequest(
            const std::filesystem::path& socketPath,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& httpResponseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request request,
            const std::function<void(std::optional<fastcgi::Response>)>& responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& httpResponseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void cancelRequest();

    template<typename TRouteContext>
    void setRequestProcessorQueue(
            const std::shared_ptr<whaleroute::RequestProcessorQueue>& queue,
            sfun::AccessPermission<RequestRouter<TRouteContext>>)
    {
        requestProcessorQueue_ = queue;
    }

private:
    detail::ResponseContext responseContext_;
    std::shared_ptr<whaleroute::RequestProcessorQueue> requestProcessorQueue_;
};

} // namespace asyncgi
