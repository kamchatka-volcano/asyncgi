#pragma once
#include "request.h"
#include "response.h"
#include "detail/irequestprocessor.h"
#include <whaleroute/requestprocessor.h>
#include <sstream>
#include <functional>

namespace asyncgi{

namespace detail{

template <typename TRouteContext = _>
class BaseRequestProcessor : public detail::IRequestProcessor{
public:
    virtual void doProcess(const Request&, Response<TRouteContext>&) = 0;

private:
    void processRequest(const Request& request, detail::ResponseContext& response) override
    {
        auto contextualResponse = Response<TRouteContext>{response};
        doProcess(request, contextualResponse);
    };
};
}

template <typename TRouteContext = _, typename... TRouteParam>
class RequestProcessor : public detail::BaseRequestProcessor<TRouteContext>,
                         public whaleroute::RequestProcessor<Request, Response<TRouteContext>, TRouteParam...>
{
private:
    void doProcess(const Request& request, Response<TRouteContext>& response) override
    {
        whaleroute::RequestProcessor<Request, Response<TRouteContext>, TRouteParam...>::processRouterRequest(request, response, {});
    }

    void onRouteParametersError(const Request&, Response<TRouteContext>& response, const whaleroute::RouteParameterError&) override
    {
        response.send(http::ResponseStatus::Code_500_Internal_Server_Error);
    };
};

}
