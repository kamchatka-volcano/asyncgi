#include <asyncgi/testserver.h>
#include <asyncgi/requestprocessor.h>
#include <asyncgi/request.h>
#include <asyncgi/response.h>
#include "timerprovider.h"
#include "client.h"
#include "asiodispatcher.h"
#include "responsesender.h"
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
    auto request = Request{fcgiRequest};
    auto ioContext = asio::io_context{};
    auto timer = detail::TimerProvider{ioContext};
    auto client = detail::Client{ioContext, {}};
    auto asioDispatcher = detail::AsioDispatcher{ioContext};
    auto responseSender = detail::ResponseSender{std::move(fcgiResponse)};
    auto response = detail::ResponseContext{responseSender, timer, client, asioDispatcher};
    requestProcessor_.processRequest(request, response);
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
    auto request = Request{fcgiRequest};
    auto ioContext = asio::io_context{};
    auto timerProvider = detail::TimerProvider{ioContext};
    auto client = detail::Client{ioContext, {}};
    auto asioDispatcher = detail::AsioDispatcher{ioContext};
    auto responseSender = detail::ResponseSender{std::move(fcgiResponse)};
    auto response = detail::ResponseContext{responseSender, timerProvider, client, asioDispatcher};
    requestProcessor_.processRequest(request, response);
    return result;
}

}
