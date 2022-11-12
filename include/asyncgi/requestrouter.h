#pragma once
#include "request.h"
#include "response.h"
#include "http/response.h"
#include <whaleroute/requestrouter.h>

namespace asyncgi{
namespace config = whaleroute::config;

template<typename TRouteContext>
class RequestRouter : public whaleroute::RequestRouter<Request,Response<TRouteContext>, http::Response> {
public:
    void operator()(const Request& request, Response<TRouteContext>& response)
    {
        auto requestProcessorQueuePtr = std::make_shared<whaleroute::RequestProcessorQueue>();
        response.setRequestProcessorQueue(requestProcessorQueuePtr);
        auto requestProcessorQueue = whaleroute::RequestRouter<
                Request,
                Response<TRouteContext>,
                http::Response>::makeRequestProcessorQueue(request, response);
        *requestProcessorQueuePtr = requestProcessorQueue;
        requestProcessorQueuePtr->launch();
    }

private:
    std::string getRequestPath(const Request& request) final
    {
        return std::string{request.path()};
    }

    void processUnmatchedRequest(const Request&, Response<TRouteContext>& response) final
    {
        response.send(http::ResponseStatus::Code_404_Not_Found);
    }

    bool isRouteProcessingFinished(const Request&, Response<TRouteContext>& response) const final
    {
        return response.isSent();
    }

    void setResponseValue(Response<TRouteContext>& response, const http::Response& httpResponse) final
    {
        response.send(httpResponse);
    }

    void onRouteParametersError(const Request&, Response<TRouteContext>& response, const whaleroute::RouteParameterError&) override
    {
        response.send(http::ResponseStatus::Code_500_Internal_Server_Error);
    };
};

}

template<typename TContext>
struct asyncgi::config::RouteSpecification<asyncgi::http::RequestMethod, asyncgi::Request, asyncgi::Response<TContext>> {
    bool operator()(asyncgi::http::RequestMethod value, const asyncgi::Request& request, asyncgi::Response<TContext>&) const
    {
        return value == request.method();
    }
};
