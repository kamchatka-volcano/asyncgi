#pragma once
#include "requestprocessor.h"
#include "request.h"
#include "response.h"
#include <whaleroute/requestrouter.h>

namespace asyncgi{

template<typename TContext>
class RequestRouter : public asyncgi::RequestProcessor,
                      public whaleroute::RequestRouter<
                              asyncgi::Request,
                              asyncgi::ContextualResponse<TContext>,
                              http::RequestMethod,
                              asyncgi::ContextualRequestProcessor<TContext>,
                              http::Response> {

    void process(const asyncgi::Request& request, asyncgi::Response& response) final
    {
        auto contextualResponse = asyncgi::ContextualResponse<TContext>{response};
        whaleroute::RequestRouter<asyncgi::Request,
                asyncgi::ContextualResponse<TContext>,
                http::RequestMethod,
                asyncgi::ContextualRequestProcessor<TContext>,
                http::Response>::process(request, contextualResponse);
    }

    std::string getRequestPath(const asyncgi::Request& request) final
    {
        return request.path();
    }

    http::RequestMethod getRequestType(const asyncgi::Request& request) final
    {
        return request.method();
    }

    void processUnmatchedRequest(const asyncgi::Request&, asyncgi::ContextualResponse<TContext>& response) final
    {
        response.send(http::ResponseStatus::Code_404_Not_Found);
    }

    bool isAccessAuthorized(const asyncgi::Request&) const final
    {
        return true;
    }

    bool isRouteProcessingFinished(const asyncgi::Request&, asyncgi::ContextualResponse<TContext>& response) const final
    {
        return response.isSent();
    }

    void setResponseValue(asyncgi::ContextualResponse<TContext>& response, const http::Response& httpResponse) final
    {
        response.send(httpResponse);
    }

    void callRequestProcessor(asyncgi::ContextualRequestProcessor<TContext>& requestProcessor, const asyncgi::Request& request,
                              asyncgi::ContextualResponse<TContext>& response) final
    {
        requestProcessor.process(request, response);
    }
};


namespace detail {
struct EmptyContext {
};
}

template<>
class RequestRouter<detail::EmptyContext> : public asyncgi::RequestProcessor,
                      public whaleroute::RequestRouter<
                              asyncgi::Request,
                              asyncgi::Response,
                              http::RequestMethod,
                              asyncgi::RequestProcessor,
                              http::Response> {

    void process(const asyncgi::Request& request, asyncgi::Response& response) final
    {
        whaleroute::RequestRouter<asyncgi::Request,
                asyncgi::Response,
                http::RequestMethod,
                asyncgi::RequestProcessor,
                http::Response>::process(request, response);
    }

    std::string getRequestPath(const asyncgi::Request& request) final
    {
        return request.path();
    }

    http::RequestMethod getRequestType(const asyncgi::Request& request) final
    {
        return request.method();
    }

    void processUnmatchedRequest(const asyncgi::Request&, asyncgi::Response& response) final
    {
        response.send(http::Response::Redirect("/"));
    }

    bool isAccessAuthorized(const asyncgi::Request&) const final
    {
        return true;
    }

    bool isRouteProcessingFinished(const asyncgi::Request&, asyncgi::Response& response) const final
    {
        return response.isSent();
    }

    void setResponseValue(asyncgi::Response& response, const http::Response& httpResponse) final
    {
        response.send(httpResponse);
    }

    void callRequestProcessor(asyncgi::RequestProcessor& requestProcessor, const asyncgi::Request& request,
                              asyncgi::Response& response) final
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

