#include <asyncgi/testserver.h>
#include <asyncgi/requestprocessor.h>
#include <asyncgi/request.h>
#include <asyncgi/response.h>
#include "timerprovider.h"
#include "requestcontext.h"
#include "client.h"
#include <fcgi_responder/request.h>
#include <fcgi_responder/response.h>
#include <sfun/string_utils.h>
#include <asio.hpp>

namespace asyncgi{
namespace str = sfun::string_utils;

class Request;
class MsgStdIn;
class MsgParams;

TestServer::TestServer(detail::IRequestProcessor& requestProcessor)
    : requestProcessor_{requestProcessor}
{
}

std::string TestServer::process(const std::map<std::string, std::string>& fcgiParams,
                                const std::string& fcgiData)
{
    auto params = std::vector<std::pair<std::string, std::string>>{fcgiParams.begin(), fcgiParams.end()};
    auto result = std::string{};
    auto fcgiRequest = fcgi::Request{std::move(params), fcgiData};
    auto fcgiResponse = fcgi::Response{
            [&result](std::string&& data, std::string&&){
                result = data;
            }};
    auto context = std::make_shared<detail::RequestContext>(std::move(fcgiRequest), std::move(fcgiResponse));
    auto request = Request{context};
    auto ioContext = asio::io_context{};
    auto timer = detail::TimerProvider{ioContext};
    auto client = detail::Client{ioContext, {}};
    auto response = detail::Response{context, timer, client};
    requestProcessor_.process(request, response);
    return result;
}

std::string TestServer::process(const http::Request& httpRequest)
{
    auto [fcgiParams, fcgiStdIn] = httpRequest.toFcgiData(http::FormType::Multipart);
    auto paramVector = std::vector<std::pair<std::string, std::string>>{fcgiParams.begin(), fcgiParams.end()};
    auto result = std::string{};
    auto fcgiRequest = fcgi::Request{std::move(paramVector), std::move(fcgiStdIn)};
    auto fcgiResponse = fcgi::Response{
            [&result](std::string&& data, std::string&&){
                result = data;
            }};
    auto context = std::make_shared<detail::RequestContext>(std::move(fcgiRequest), std::move(fcgiResponse));
    auto request = Request{context};
    auto ioContext = asio::io_context{};
    auto timerProvider = detail::TimerProvider{ioContext};
    auto client = detail::Client{ioContext, {}};
    auto response = detail::Response{context, timerProvider, client};
    requestProcessor_.process(request, response);
    return result;
}

}
