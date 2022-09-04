#pragma once
#include "requestprocessor.h"
#include "request.h"
#include "response.h"
#include <hot_teacup/response.h>
#include <whaleroute/requestrouter.h>

namespace asyncgi{
namespace config = whaleroute::config;

template<typename TRouteContext>
class RequestRouter : public RequestProcessor<TRouteContext>,
                      public whaleroute::RequestRouter<
                              Request,
                              Response<TRouteContext>,
                              http::Response> {

    void process(const Request& request, Response<TRouteContext>& response) final
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
};

}

template<typename TContext>
struct asyncgi::config::RouteSpecification<http::RequestMethod, asyncgi::Request, asyncgi::Response<TContext>> {
    bool operator()(http::RequestMethod value, const asyncgi::Request& request, asyncgi::Response<TContext>&) const
    {
        return value == request.method();
    }
};
