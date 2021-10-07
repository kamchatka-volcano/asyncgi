#pragma once
#include "requestprocessor.h"
#include "request.h"
#include "response.h"
#include <whaleroute/requestrouter.h>

namespace asyncgi{
class RequestRouter : public asyncgi::RequestProcessor,
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
        response << http::Response::Redirect("/");
    }

    bool isAccessAuthorized(const asyncgi::Request&) const final
    {
        return true;
    }

    void setResponseValue(asyncgi::Response& response, const http::Response& httpResponse) final
    {
        response << httpResponse;
    }

    void callRequestProcessor(asyncgi::RequestProcessor& requestProcessor, const asyncgi::Request& request,
                              asyncgi::Response& response) final
    {
        requestProcessor.process(request, response);
    }

};
}

///
/// using Router = clean_route::Router<asyncgi::Request, http::RequestMethod, asyncgi::Response, asyncgi::RequestProcessor>;
/// auto router = Router([](const http::Request& request){ return request.path();},
///                      [](const http::Request& request){ return request.method();},
///                      [](http::Response& response){response.set()});

///


