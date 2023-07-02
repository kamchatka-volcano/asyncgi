#include "asiodispatcherservice.h"
#include "clientservice.h"
#include "responsecontext.h"
#include "responsesender.h"
#include "timerprovider.h"
#include <asyncgi/http/request.h>
#include <asyncgi/http/response.h>
#include <asyncgi/response.h>
#include <asyncgi/types.h>
#include <functional>
#include <memory>
#include <optional>

namespace asyncgi {

namespace detail {
class TimerProvider;
} // namespace detail

Response::Response(detail::ResponseContext& responseContext)
    : responseContext_{responseContext}
{
}

void Response::redirect(
        std::string path,
        http::RedirectType redirectType,
        std::vector<http::Cookie> cookies,
        std::vector<http::Header> headers)
{
    auto response = http::Response{std::move(path), redirectType, std::move(cookies), std::move(headers)};
    responseContext_.get().responseSender().send(response.data(http::ResponseMode::Cgi));
}

void Response::send(const http::Response& response)
{
    responseContext_.get().responseSender().send(response.data(http::ResponseMode::Cgi));
}

void Response::send(fastcgi::Response response)
{
    responseContext_.get().responseSender().send(std::move(response.data), std::move(response.errorMsg));
}

bool Response::isSent() const
{
    return responseContext_.get().responseSender().isSent();
}

detail::ResponseContext& Response::context(detail::ResponseContextAccessPermission)
{
    return responseContext_;
}

} // namespace asyncgi
