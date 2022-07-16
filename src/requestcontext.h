#pragma once
#include <hot_teacup/request_view.h>
#include <fcgi_responder/request.h>
#include <fcgi_responder/response.h>
#include <memory>
#include <optional>

namespace asyncgi::detail{

class RequestContext{
public:
    RequestContext(fcgi::Request&& request, fcgi::Response&& response);
    const http::RequestView& request();
    fcgi::Response& response();
    const fcgi::Request& fcgiRequest() const;

private:
    fcgi::Response response_;
    fcgi::Request  fcgiRequest_;
    std::optional<http::RequestView> request_;
};

}
