#pragma once
#include "itimer.h"
#include "iclient.h"
#include "types.h"
#include <hot_teacup/response.h>
#include <hot_teacup/request.h>
#include <whaleroute/requestprocessorqueue.h>
#include <memory>
#include <optional>
#include <future>

namespace asyncgi{
class TimerFactory;

namespace detail {
class RequestContext;

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

namespace detail {
class Response{
public:
    Response(std::shared_ptr<RequestContext> context, ITimer& timer, IClient& client);
    void send(const http::Response&);
    bool isSent() const;
    ITimer& timer();
    IClient& client();

private:
    std::shared_ptr<RequestContext> context_;
    std::reference_wrapper<ITimer> timer_;
    std::reference_wrapper<IClient> client_;
};
}

template <typename TResponseContext = detail::EmptyContext>
class Response{
public:
    explicit Response(detail::Response response)
            : response_(std::move(response))
    {}

    TResponseContext& context()
    {
        return responseContext_;
    }

    void send(const http::Response& response)
    {
        response_.send(response);
    }

    bool isSent() const
    {
        return response_.isSent();
    }

    void setRequestProcessorQueue(const std::shared_ptr<whaleroute::RequestProcessorQueue>& queue)
    {
        requestProcessorQueue_ = queue;
    }

    template <typename T, typename TCallable>
    void waitFuture(std::future<T>&& future, TCallable callback, std::chrono::milliseconds checkPeriod = std::chrono::milliseconds{1})
    {
        static_assert (std::is_invocable_v<TCallable, T>, "TCallable must be invokable with argument of type T");
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();
        auto& timer = response_.timer();
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

    void makeRequest(const std::filesystem::path& socketPath,
                     const FCGIRequest& fcgiRequest,
                     const std::function<void(const std::optional<FCGIResponse>&)>& responseHandler)
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();

        response_.client().makeRequest(socketPath, fcgiRequest,
           [this, responseHandler](const std::optional<FCGIResponse>& response){
               if (response)
                   responseHandler(response);
               else
                   responseHandler(std::nullopt);
               if (requestProcessorQueue_)
                   requestProcessorQueue_->launch();
           });
    }

    void makeRequest(const std::filesystem::path& socketPath,
                     const http::Request& request,
                     const std::function<void(const std::optional<http::Response>&)>& httpResponseHandler)
    {
        if (requestProcessorQueue_)
            requestProcessorQueue_->stop();

        auto fcgiRequestData = request.toFcgiData(http::FormType::Multipart);
        auto fcgiRequest = FCGIRequest{std::move(fcgiRequestData.params), std::move(fcgiRequestData.stdIn)};
        response_.client().makeRequest(socketPath, fcgiRequest,
           [this, httpResponseHandler](const std::optional<FCGIResponse>& response){
               if (response)
                   httpResponseHandler(http::responseFromString(response->data));
               else
                   httpResponseHandler(std::nullopt);
               if (requestProcessorQueue_)
                   requestProcessorQueue_->launch();
           });
    }

    void cancelRequest()
    {
        response_.client().disconnect();
    }

private:
    detail::Response response_;
    TResponseContext responseContext_;
    std::shared_ptr<whaleroute::RequestProcessorQueue> requestProcessorQueue_;
};


}
