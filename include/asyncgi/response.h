#pragma once
#include "iasiodispatcher.h"
#include "itimer.h"
#include "iclient.h"
#include "types.h"
#include "detail/iresponsesender.h"
#include "detail/responsecontext.h"
#include <hot_teacup/response.h>
#include <hot_teacup/request.h>
#include <whaleroute/requestprocessorqueue.h>
#include <memory>
#include <optional>
#include <future>
#include <functional>

namespace asio{
    class io_context;
}

namespace asyncgi{
template<typename TContext>
class RequestRouter;

namespace detail {
class TimerProvider;

template<class F>
auto make_copyable_function(F&& f)
{
    using dF = std::decay_t<F>;
    auto spf = std::make_shared<dF>(std::forward<F>(f));
    return [spf](auto&& ... args) -> decltype(auto) {
        return (*spf)(decltype(args)(args)...);
    };
}
}

template <typename TRouteContext = detail::EmptyRouteContext>
class Response{
public:
    explicit Response(const detail::ResponseContext& responseContext)
            : responseContext_{responseContext}
    {}

    TRouteContext& context()
    {
        return routeContext_;
    }

    void send(const http::Response& response)
    {
        responseContext_.responseSender().send(response.data(http::ResponseMode::CGI));
    }

    bool isSent() const
    {
        return responseContext_.responseSender().isSent();
    }

    void executeTask(std::function<void(const TaskContext& ctx)> task)
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();

        responseContext_.asioDispatcher().postTask(std::move(task),
                [this]{
                    if (requestProcessorQueue_)
                        requestProcessorQueue_->launch();
                });
    }

    template <typename T, typename TCallable>
    void waitFuture(std::future<T>&& future, TCallable callback, std::chrono::milliseconds checkPeriod = std::chrono::milliseconds{1})
    {
        static_assert (std::is_invocable_v<TCallable, T>, "TCallable must be invokable with argument of type T");
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();
        auto& timer = responseContext_.makeTimer();
        auto timerCallback = [fut = std::move(future), func = std::move(callback), &timer, checkPeriod, this]() mutable{
            if (fut.wait_for(std::chrono::seconds{0}) == std::future_status::ready){
                timer.stop();
                func(fut.get());
                if (requestProcessorQueue_)
                    requestProcessorQueue_->launch();
            }
            else
                waitFuture(std::move(fut), std::move(func), checkPeriod);
        };
        timer.start(checkPeriod, detail::make_copyable_function(std::move(timerCallback)), TimerMode::Once);
    }

    void makeRequest(
            const std::filesystem::path& socketPath,
            const std::map<std::string, std::string>& fcgiParams,
            const std::string& fcgiStdIn,
            const std::function<void(const std::optional<std::string>&)>& responseHandler,
            const std::chrono::milliseconds timeout = std::chrono::seconds{3})
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();

        responseContext_.client().makeRequest(socketPath, fcgiParams, fcgiStdIn,
                                              [this, responseHandler](const std::optional<std::string>& response){
               if (response)
                   responseHandler(*response);
               else
                   responseHandler(std::nullopt);
               if (requestProcessorQueue_)
                   requestProcessorQueue_->launch();
           }, timeout);
    }

    void makeRequest(
            const std::filesystem::path& socketPath,
            const http::Request& request,
            const std::function<void(const std::optional<http::ResponseView>&)>& httpResponseHandler,
            const std::chrono::milliseconds timeout = std::chrono::seconds{3})
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();

        auto fcgiRequest = request.toFcgiData(http::FormType::Multipart);
        responseContext_.client().makeRequest(socketPath, fcgiRequest.params, fcgiRequest.stdIn,
                                              [this, httpResponseHandler](const std::optional<std::string>& response){
               if (response)
                   httpResponseHandler(http::responseFromString(*response));
               else
                   httpResponseHandler(std::nullopt);
               if (requestProcessorQueue_)
                   requestProcessorQueue_->launch();
           }, timeout);
    }

        void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const std::map<std::string, std::string>& fcgiParams,
            const std::string& fcgiStdIn,
            const std::function<void(const std::optional<std::string>&)>& responseHandler,
            const std::chrono::milliseconds timeout = std::chrono::seconds{3})
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();

        responseContext_.client().makeRequest(ipAddress, port, fcgiParams, fcgiStdIn,
                                              [this, responseHandler](const std::optional<std::string>& response){
               if (response)
                   responseHandler(*response);
               else
                   responseHandler(std::nullopt);
               if (requestProcessorQueue_)
                   requestProcessorQueue_->launch();
           }, timeout);
    }

    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(const std::optional<http::ResponseView>&)>& httpResponseHandler,
            const std::chrono::milliseconds timeout = std::chrono::seconds{3})
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();

        auto fcgiRequest = request.toFcgiData(http::FormType::Multipart);
        responseContext_.client().makeRequest(ipAddress, port, fcgiRequest.params, fcgiRequest.stdIn,
                                              [this, httpResponseHandler](const std::optional<std::string>& response){
               if (response)
                   httpResponseHandler(http::responseFromString(*response));
               else
                   httpResponseHandler(std::nullopt);
               if (requestProcessorQueue_)
                   requestProcessorQueue_->launch();
           }, timeout);
    }

    void cancelRequest()
    {
        responseContext_.client().disconnect();
    }

private:
    void setRequestProcessorQueue(const std::shared_ptr<whaleroute::RequestProcessorQueue>& queue)
    {
        requestProcessorQueue_ = queue;
    }

private:
    detail::ResponseContext responseContext_;
    TRouteContext routeContext_;
    std::shared_ptr<whaleroute::RequestProcessorQueue> requestProcessorQueue_;

    friend class RequestRouter<TRouteContext>;
};


}
