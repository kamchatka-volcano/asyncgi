#pragma once
#include "requestprocessor.h"
#include "request.h"
#include "response.h"
#include <hot_teacup/response.h>
#include <whaleroute/requestrouter.h>

namespace asyncgi{

template<typename TContext>
class RequestRouter : public asyncgi::RequestProcessor<TContext>,
                      public whaleroute::RequestRouter<
                              asyncgi::Request,
                              asyncgi::Response<TContext>,
                              http::RequestMethod,
                              asyncgi::RequestProcessor<TContext>,
                              http::Response> {

    void process(const asyncgi::Request& request, asyncgi::Response<TContext>& response) final
    {
        auto requestProcessorQueuePtr = std::make_shared<whaleroute::RequestProcessorQueue>();
        response.setRequestProcessorQueue(requestProcessorQueuePtr);
        auto requestProcessorQueue = whaleroute::RequestRouter<asyncgi::Request,
                asyncgi::Response<TContext>,
                http::RequestMethod,
                asyncgi::RequestProcessor<TContext>,
                http::Response>::makeRequestProcessorQueue(request, response);
        *requestProcessorQueuePtr = requestProcessorQueue;
        requestProcessorQueuePtr->launch();
    }

    std::string getRequestPath(const asyncgi::Request& request) final
    {
        return request.path();
    }

    http::RequestMethod getRequestType(const asyncgi::Request& request) final
    {
        return request.method();
    }

    void processUnmatchedRequest(const asyncgi::Request&, asyncgi::Response<TContext>& response) final
    {
        response.send(http::ResponseStatus::Code_404_Not_Found);
    }

    bool isAccessAuthorized(const asyncgi::Request&) const final
    {
        return true;
    }

    bool isRouteProcessingFinished(const asyncgi::Request&, asyncgi::Response<TContext>& response) const final
    {
        return response.isSent();
    }

    void setResponseValue(asyncgi::Response<TContext>& response, const http::Response& httpResponse) final
    {
        response.send(httpResponse);
    }

    void callRequestProcessor(asyncgi::RequestProcessor<TContext>& requestProcessor, const asyncgi::Request& request,
                              asyncgi::Response<TContext>& response) final
    {
        requestProcessor.process(request, response);
    }
};


template<typename TContext = detail::EmptyContext>
RequestRouter<TContext> makeRouter()
{
    return RequestRouter<TContext>{};
}

}

