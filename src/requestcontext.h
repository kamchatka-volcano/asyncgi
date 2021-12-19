#pragma once
#include <hot_teacup/request.h>
#include <fcgi_responder/request.h>
#include <fcgi_responder/response.h>
#include <memory>

namespace asyncgi::detail{

class RequestContext{
public:
    RequestContext(fcgi::Request&& request, fcgi::Response&& response);
    const http::Request& request() const;
    fcgi::Response& response();
    const fcgi::Request& fcgiRequest() const;

private:
    fcgi::Response response_;
    fcgi::Request  fcgiRequest_;
    http::Request request_;
};

}
