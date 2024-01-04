#ifndef ASYNCGI_ROUTER_H
#define ASYNCGI_ROUTER_H

#include "errors.h"
#include "io.h"
#include "request.h"
#include "response.h"
#include "types.h"
#include "detail/external/sfun/functional.h"
#include "detail/external/sfun/interface.h"
#include "detail/external/sfun/type_traits.h"
#include "detail/external/whaleroute/requestrouter.h"
#include "detail/routeresponsecontextaccessor.h"
#include "http/response.h"

namespace asyncgi {
namespace config = whaleroute::config;

template<typename TRouteContext>
class Router;

class RouterIOAccess {
public:
    template<typename TRouteContext>
    static auto makeToken(sfun::access_permission<Router<TRouteContext>>)
    {
        return sfun::access_token<RouterIOAccess>{};
    }
};

namespace detail {
struct ResponseSender {
    template<typename TResponse>
    void operator()(Responder& responseSender, const TResponse& response)
    {
        if constexpr (sfun::is_optional_v<TResponse>) {
            if (response.has_value())
                responseSender.send(response.value());
        }
        else {
            responseSender.send(response);
        }
    }

    template<typename... TResponse>
    auto operator()(Responder& responseSender, TResponse&&... response) -> std::enable_if_t<(sizeof...(TResponse) > 1)>
    {
        responseSender.send(response...);
    }
};
} //namespace detail

template<typename TRouteContext = _>
class Router : public whaleroute::RequestRouter<Request, Responder, detail::ResponseSender, TRouteContext> {
public:
    explicit Router(IO& io)
        : eventHandler_{io.eventHandler(RouterIOAccess::makeToken<TRouteContext>(sfun::access_token{*this}))}
    {
    }

    void operator()(const Request& request, Responder& response)
    {
        auto requestProcessorQueuePtr = std::make_shared<whaleroute::RequestProcessorQueue>();
        detail::RouterResponseContextAccessor::setRequestProcessorQueue(response, requestProcessorQueuePtr);
        auto requestProcessorQueue =
                whaleroute::RequestRouter<asyncgi::Request, asyncgi::Responder, detail::ResponseSender, TRouteContext>::
                        makeRequestProcessorQueue(request, response);

        *requestProcessorQueuePtr = requestProcessorQueue;
        requestProcessorQueuePtr->launch();
    }

private:
    std::string getRequestPath(const Request& request) final
    {
        return std::string{request.path()};
    }

    void processUnmatchedRequest(const Request&, Responder& response) final
    {
        response.send(http::ResponseStatus::_404_Not_Found);
    }

    bool isRouteProcessingFinished(const Request&, Responder& response) const final
    {
        return response.isSent();
    }

    void onRouteParametersError(const Request&, Responder& response, const whaleroute::RouteParameterError& error)
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
        eventHandler_(RouteParametersError, std::visit(errorMessageVisitor, error));
        response.send(http::ResponseStatus::_500_Internal_Server_Error);
    };

private:
    detail::EventHandlerProxy eventHandler_;
};

template<>
struct config::RouteMatcher<asyncgi::http::RequestMethod> {
    bool operator()(asyncgi::http::RequestMethod value, const asyncgi::Request& request) const
    {
        return value == request.method();
    }
};

template<typename TContext>
struct config::RouteMatcher<asyncgi::http::RequestMethod, TContext> {
    bool operator()(asyncgi::http::RequestMethod value, const asyncgi::Request& request, const TContext&) const
    {
        return value == request.method();
    }
};

} // namespace asyncgi

#endif //ASYNCGI_ROUTER_H