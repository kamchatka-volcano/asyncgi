#pragma once
#include "requestprocessor.h"
#include "request.h"
#include "response.h"
#include <hot_teacup/response.h>
#include <whaleroute/requestrouter.h>

namespace asyncgi{
namespace traits = whaleroute::traits;

template<typename TRouteContext>
class RequestRouter : public asyncgi::RequestProcessor<TRouteContext>,
                      public whaleroute::RequestRouter<
                              asyncgi::Request,
                              asyncgi::Response<TRouteContext>,
                              asyncgi::RequestProcessor<TRouteContext>,
                              http::Response> {

    void process(const asyncgi::Request& request, asyncgi::Response<TRouteContext>& response) final
    {
        auto requestProcessorQueuePtr = std::make_shared<whaleroute::RequestProcessorQueue>();
        response.setRequestProcessorQueue(requestProcessorQueuePtr);
        auto requestProcessorQueue = whaleroute::RequestRouter<asyncgi::Request,
                asyncgi::Response<TRouteContext>,
                asyncgi::RequestProcessor<TRouteContext>,
                http::Response>::makeRequestProcessorQueue(request, response);
        *requestProcessorQueuePtr = requestProcessorQueue;
        requestProcessorQueuePtr->launch();
    }

    std::string getRequestPath(const asyncgi::Request& request) final
    {
        return std::string{request.path()};
    }

    void processUnmatchedRequest(const asyncgi::Request&, asyncgi::Response<TRouteContext>& response) final
    {
        response.send(http::ResponseStatus::Code_404_Not_Found);
    }

    bool isRouteProcessingFinished(const asyncgi::Request&, asyncgi::Response<TRouteContext>& response) const final
    {
        return response.isSent();
    }

    void setResponseValue(asyncgi::Response<TRouteContext>& response, const http::Response& httpResponse) final
    {
        response.send(httpResponse);
    }

    void callRequestProcessor(asyncgi::RequestProcessor<TRouteContext>& requestProcessor, const asyncgi::Request& request,
                              asyncgi::Response<TRouteContext>& response) final
    {
        requestProcessor.process(request, response);
    }
};

}

template<typename TContext>
struct asyncgi::traits::RouteSpecification<http::RequestMethod, asyncgi::Request, asyncgi::Response<TContext>> {
    bool operator()(http::RequestMethod value, const asyncgi::Request& request, asyncgi::Response<TContext>&) const
    {
        return value == request.method();
    }
};
