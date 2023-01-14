#pragma once
#include "request.h"
#include "response.h"
#include "detail/external/sfun/functional.h"
#include "detail/external/sfun/interface.h"
#include "detail/external/whaleroute/requestrouter.h"
#include "http/response.h"

namespace asyncgi {
namespace config = whaleroute::config;

template<typename TRouteContext>
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

    void onRouteParametersError(const Request&, Response& response, const whaleroute::RouteParameterError& error)
            override
    {
        auto errorMessageVisitor = sfun::overloaded{
                [](const whaleroute::RouteParameterCountMismatch& countMismatchError) -> std::string
                {
                    return "RouteParameterError: Parameter count mismatch, expected: " +
                            std::to_string(countMismatchError.expectedNumber) +
                            ", actual:" + std::to_string(countMismatchError.actualNumber);
                },
                [](const whaleroute::RouteParameterReadError& readError) -> std::string
                {
                    return "RouteParameterError: Couldn't read parameter#" + std::to_string(readError.index) +
                            ", value:" + readError.value;
                },

        };
        response.send(http::ResponseStatus::Code_500_Internal_Server_Error, std::visit(errorMessageVisitor, error));
    };
};

template<>
struct config::RouteMatcher<asyncgi::http::RequestMethod> {
    bool operator()(asyncgi::http::RequestMethod value, const asyncgi::Request& request, asyncgi::Response&) const
    {
        return value == request.method();
    }
};

template<typename TContext>
struct config::RouteMatcher<asyncgi::http::RequestMethod, TContext> {
    bool operator()(asyncgi::http::RequestMethod value, const asyncgi::Request& request, asyncgi::Response&, TContext&)
            const
    {
        return value == request.method();
    }
};

} // namespace asyncgi
