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

Response::Response(std::shared_ptr<detail::ResponseContext> responseContext)
    : responseContext_{responseContext}
{
}

void Response::redirect(
        std::string path,
        http::RedirectType redirectType,
        std::vector<http::Cookie> cookies,
        std::vector<http::Header> headers)
{
    if (auto context = responseContext_.lock()) {
        auto response = http::Response{std::move(path), redirectType, std::move(cookies), std::move(headers)};
        context->responseSender().send(response.data(http::ResponseMode::Cgi));
    }
}

void Response::send(const http::Response& response)
{
    if (auto context = responseContext_.lock())
        context->responseSender().send(response.data(http::ResponseMode::Cgi));
}

void Response::send(fastcgi::Response response)
{
    if (auto context = responseContext_.lock())
        context->responseSender().send(std::move(response.data), std::move(response.errorMsg));
}

bool Response::isSent() const
{
    if (auto context = responseContext_.lock())
        return context->responseSender().isSent();

    return true;
}

std::weak_ptr<detail::ResponseContext> Response::context(detail::ResponseContextAccessPermission)
{
    return responseContext_;
}

} // namespace asyncgi
