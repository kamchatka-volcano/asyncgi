#pragma once
#include "request.h"
#include "response.h"
#include "detail/external/sfun/interface.h"
#include "detail/external/whaleroute/requestrouter.h"
#include "http/response.h"

namespace asyncgi {
namespace config = whaleroute::config;

template <typename TRouteContext>
class RequestRouter : public whaleroute::RequestRouter<Request, Response, http::Response, TRouteContext> {
public:
    void operator()(const Request& request, Response& response)
    {
        auto requestProcessorQueuePtr = std::make_shared<whaleroute::RequestProcessorQueue>();
        response.setRequestProcessorQueue<TRouteContext>(requestProcessorQueuePtr, sfun::AccessToken{*this});
        auto requestProcessorQueue =
                whaleroute::RequestRouter<asyncgi::Request, asyncgi::Response, http::Response, TRouteContext>::
                        makeRequestProcessorQueue(request, response);
        *requestProcessorQueuePtr = requestProcessorQueue;
        requestProcessorQueuePtr->launch();
    }

private:
    std::string getRequestPath(const Request& request) final
    {
        return std::string{request.path()};
    }

    void processUnmatchedRequest(const Request&, Response& response) final
    {
        response.send(http::ResponseStatus::Code_404_Not_Found);
    }

    bool isRouteProcessingFinished(const Request&, Response& response) const final
    {
        return response.isSent();
    }

    void setResponseValue(Response& response, const http::Response& httpResponse) final
    {
        response.send(httpResponse);
    }

    void onRouteParametersError(const Request&, Response& response, const whaleroute::RouteParameterError&) override
    {
        response.send(http::ResponseStatus::Code_500_Internal_Server_Error);
    };
};

} // namespace asyncgi

template <typename TContext>
struct asyncgi::config::
        RouteSpecification<asyncgi::http::RequestMethod, asyncgi::Request, asyncgi::Response, TContext> {

    template <typename T = TContext, std::enable_if_t<std::is_same_v<T, _>>* = nullptr>
    bool operator()(asyncgi::http::RequestMethod value, const asyncgi::Request& request, asyncgi::Response&) const
    {
        return value == request.method();
    }

    template <typename T = TContext, std::enable_if_t<!std::is_same_v<T, _>>* = nullptr>
    bool operator()(asyncgi::http::RequestMethod value, const asyncgi::Request& request, asyncgi::Response&, TContext&)
            const
    {
        return value == request.method();
    }
};
