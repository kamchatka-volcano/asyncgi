#ifndef ASYNCGI_ROUTER_H
#define ASYNCGI_ROUTER_H

#include "errors.h"
#include "io.h"
#include "responder.h"
#include "types.h"
#include "detail/external/sfun/functional.h"
#include "detail/external/sfun/interface.h"
#include "detail/external/sfun/string_utils.h"
#include "detail/external/sfun/type_traits.h"
#include "detail/external/whaleroute/requestrouter.h"
#include "detail/external/whaleroute/routeparam.h"
#include "detail/routeresponsecontextaccessor.h"
#include "http/request.h"
#include "http/response.h"
#include <unordered_map>

namespace asyncgi {
namespace config = whaleroute::config;

constexpr auto routeParamId(std::string_view paramTypeName)
{
    return whaleroute::routeParamId(paramTypeName);
}

template<>
struct config::RouteParam<routeParamId("int")> {
    using type = int;
    inline static std::string_view name = "int";
    inline static std::string_view regex = R"(\d+)";
};

template<>
struct config::RouteParam<routeParamId("str")> {
    using type = std::string;
    inline static std::string_view name = "str";
    inline static std::string_view regex = R"([\w\$-\.\+!*'\(\)]+)";
};

template<>
struct config::RouteParam<routeParamId("any")> {
    using type = std::string;
    inline static std::string_view name = "any";
    inline static std::string_view regex = R"([\w\$-\.\+!*'\(\)/]+)";
};

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
class Router : public whaleroute::RequestRouter<http::Request, Responder, detail::ResponseSender, TRouteContext> {
public:
    explicit Router(IO& io)
        : eventHandler_{io.eventHandler(RouterIOAccess::makeToken<TRouteContext>(sfun::access_token{*this}))}
    {
    }

    void operator()(const http::Request& request, Responder& response)
    {
        auto requestProcessorQueuePtr = std::make_shared<whaleroute::RequestProcessorQueue>();
        detail::RouterResponseContextAccessor::setRequestProcessorQueue(response, requestProcessorQueuePtr);
        auto requestProcessorQueue =
                whaleroute::RequestRouter<http::Request, asyncgi::Responder, detail::ResponseSender, TRouteContext>::
                        makeRequestProcessorQueue(request, response);

        *requestProcessorQueuePtr = requestProcessorQueue;
        requestProcessorQueuePtr->launch();
    }

private:
    std::string getRequestPath(const http::Request& request) final
    {
        return std::string{request.path()};
    }

    void processUnmatchedRequest(const http::Request&, Responder& response) final
    {
        response.send(http::ResponseStatus::_404_Not_Found);
    }

    bool isRouteProcessingFinished(const http::Request&, Responder& response) const final
    {
        return response.isSent();
    }

    void onRouteParametersError(const http::Request&, Responder& response, const whaleroute::RouteParameterError& error)
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

    void onUnregisteredRouteParameterError(std::string_view paramName) const final
    {
        throw Error{sfun::join_strings(
                "Regular expression for route parameter '",
                paramName,
                "' isn't registered. Override asyncgi::Router::routeParamRegex() method to add it.")};
    }

    std::optional<std::string_view> routeParamRegex(std::string_view paramName) const final
    {
        static const auto routeParamRegex = std::unordered_map<std::string, std::string>{
                {"int", R"(\d+)"},
                {"str", R"([\w\$-\.\+!*'\(\)]+)"},
                {"any", R"([\w\$-\.\+!*'\(\)/]+)"}};

        auto it = routeParamRegex.find(std::string{paramName});
        if (it == routeParamRegex.end())
            return std::nullopt;
        return it->second;
    }

private:
    detail::EventHandlerProxy eventHandler_;
};

template<>
struct config::RouteMatcher<asyncgi::http::RequestMethod> {
    bool operator()(asyncgi::http::RequestMethod value, const http::Request& request) const
    {
        return value == request.method();
    }
};

template<typename TContext>
struct config::RouteMatcher<asyncgi::http::RequestMethod, TContext> {
    bool operator()(asyncgi::http::RequestMethod value, const http::Request& request, const TContext&) const
    {
        return value == request.method();
    }
};

} // namespace asyncgi

#endif //ASYNCGI_ROUTER_H