#ifdef ASYNCGI_USE_HTTP_CLIENT

#include "asiodispatcherservice.h"
#include "ioservice.h"
#include "responsecontext.h"
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/detail/external/sfun/optional_ref.h>
#include <asyncgi/http/request.h>
#include <asyncgi/http/response.h>
#include <asyncgi/http/response_cookie_view.h>
#include <asyncgi/http_client.h>
#include <asyncgi/io.h>
#include <asyncgi/responder.h>
#include <sfun/string_utils.h>
#include <restc-cpp/restc-cpp.h>

namespace asyncgi {

HttpClient::HttpClient(IO& io)
    : asioDispatcherService_{
              std::make_unique<detail::AsioDispatcherService>(io.ioService(sfun::access_token<HttpClient>{}).io())}
    , requestErrorMessage_{std::make_shared<std::string>()}
{
}

namespace {
sfun::optional_ref<detail::AsioDispatcherService> getAsioDispatcherService(
        Responder& responder,
        sfun::access_token<HttpClient> accessToken)
{
    if (auto context = responder.context(accessToken).lock())
        return context->asioDispatcher();
    return std::nullopt;
}
} //namespace

HttpClient::HttpClient(Responder& responder)
    : asioDispatcherService_{getAsioDispatcherService(responder, sfun::access_token<HttpClient>{})}
{
}

namespace {

restc_cpp::Request::Properties makeProperties(const http::Request& request, std::chrono::milliseconds timeout)
{
    auto props = restc_cpp::Request::Properties{};
    for (const auto& header : request.headers()) {
        const auto headerStr = header.toString();
        const auto value = std::string{sfun::after(headerStr, ":").value()};
        props.headers.emplace(header.name(), value);
    }
    if (!request.cookies().empty())
        props.headers.emplace("Cookie", http::requestCookiesToHeaderValueString(request.cookies()));

    for (const auto& query : request.queries())
        props.args.emplace_back(std::string{query.name()}, std::string{query.value()});

    props.replyTimeoutMs = timeout.count();
    return props;
}

std::unique_ptr<restc_cpp::Reply> sendRequest(const http::Request& request, restc_cpp::Context& ctx)
{
    switch (request.method()) {
    case http::RequestMethod::Get:
        return ctx.Get(std::string{request.path()});
    case http::RequestMethod::Head:
        return ctx.Head(std::string{request.path()});
    case http::RequestMethod::Post:
        return ctx.Post(std::string{request.path()}, std::string{request.body()});
    case http::RequestMethod::Put:
        return ctx.Put(std::string{request.path()}, std::string{request.body()});
    case http::RequestMethod::Delete:
        return ctx.Delete(std::string{request.path()});
    case http::RequestMethod::Options:
        return ctx.Options(std::string{request.path()});
    case http::RequestMethod::Patch:
        return ctx.Patch(std::string{request.path()});
    default:
        throw Error{"Unsupported HTTP method"};
    }
}

http::Response makeResponse(restc_cpp::Reply& reply)
{
    auto response = http::Response{http::statusFromCode(reply.GetResponseCode()), reply.GetBodyAsString()};
    auto cookies = std::vector<http::ResponseCookie>{};
    auto headers = std::vector<http::Header>{};
    for (const auto headerName : reply.headerNames()) {
        if (headerName == "Set-Cookie") {
            const auto cookieHeaderResult =
                    http::headerFromValueString(headerName, reply.header(std::string{headerName}).value());
            if (cookieHeaderResult.has_value()) {
                const auto cookie = http::responseCookieFromHeader(cookieHeaderResult.value());
                if (cookie.has_value())
                    cookies.emplace_back(std::move(cookie.value()));
            }
        }
        else {
            const auto headerResult =
                    http::headerFromValueString(headerName, reply.header(std::string{headerName}).value());
            if (headerResult.has_value())
                headers.emplace_back(std::move(headerResult.value()));
        }
    }
    response.setCookies(cookies);
    response.setHeaders(headers);
    return response;
}

} //namespace

void HttpClient::makeRequest(
        const http::Request& request,
        const std::function<void(std::optional<http::Response>)>& responseHandler,
        std::chrono::milliseconds timeout)
{
    if (!asioDispatcherService_.has_value())
        return;

    const auto setErrorMessage = [msg = std::weak_ptr{requestErrorMessage_}](const std::string& errorMessage)
    {
        if (const auto msgPtr = msg.lock())
            *msgPtr = errorMessage;
    };

    asioDispatcherService_.get().postTask(
            [request, responseHandler, timeout, setErrorMessage](const AsioContext& taskContext)
            {
                auto makeResponseFunc = std::make_shared<std::function<http::Response()>>();
                const auto requestProperties = makeProperties(request, timeout);
                auto restClient = std::shared_ptr{restc_cpp::RestClient::Create(requestProperties, taskContext.io())};
                restClient->ProcessWithPromise(
                        [restClient, request, makeResponseFunc, responseHandler, setErrorMessage](
                                restc_cpp::Context& ctx)
                        {
                            try {
                                auto reply = sendRequest(request, ctx);
                                *makeResponseFunc = [response = makeResponse(*reply)]
                                {
                                    return response;
                                };
                                setErrorMessage("");
                            }
                            catch (const std::exception& e) {
                                setErrorMessage(e.what());
                                responseHandler(std::nullopt);
                            }
                        },
                        [responseHandler, makeResponseFunc](const std::future<void>&)
                        {
                            responseHandler((*makeResponseFunc)());
                        });
            });
}

const std::string& HttpClient::requestErrorMessage() const
{
    return *requestErrorMessage_;
}

} //namespace asyncgi

#endif //ASYNCGI_USE_HTTP_CLIENT