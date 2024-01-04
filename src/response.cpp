#include "asiodispatcherservice.h"
#include "clientservice.h"
#include "responsecontext.h"
#include "responsesender.h"
#include "timerprovider.h"
#include <asyncgi/http/request.h>
#include <asyncgi/http/response.h>
#include <asyncgi/responder.h>
#include <asyncgi/types.h>
#include <functional>
#include <memory>
#include <optional>

namespace asyncgi {

namespace detail {
class TimerProvider;
} // namespace detail

Responder::Responder(std::shared_ptr<detail::ResponseContext> responseContext)
    : responseContext_{responseContext}
{
}

void Responder::send(const http::Response& response)
{
    if (auto context = responseContext_.lock())
        context->responseSender().send(response.data(http::ResponseMode::Cgi));
}

void Responder::send(fastcgi::Response response)
{
    if (auto context = responseContext_.lock())
        context->responseSender().send(std::move(response.data), std::move(response.errorMsg));
}

bool Responder::isSent() const
{
    if (auto context = responseContext_.lock())
        return context->responseSender().isSent();

    return true;
}

std::weak_ptr<detail::ResponseContext> Responder::context(detail::ResponseContextAccessPermission)
{
    return responseContext_;
}

} // namespace asyncgi
