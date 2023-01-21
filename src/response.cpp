#include <asyncgi/detail/iresponsesender.h>
#include <asyncgi/detail/responsecontext.h>
#include <asyncgi/http/request.h>
#include <asyncgi/http/response.h>
#include <asyncgi/iasiodispatcher.h>
#include <asyncgi/iclient.h>
#include <asyncgi/itimer.h>
#include <asyncgi/response.h>
#include <asyncgi/types.h>
#include <functional>
#include <memory>
#include <optional>

namespace asyncgi {

namespace detail {
class TimerProvider;
} // namespace detail

Response::Response(const detail::ResponseContext& responseContext)
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
    responseContext_.responseSender().send(response.data(http::ResponseMode::Cgi));
}

void Response::send(const http::Response& response)
{
    responseContext_.responseSender().send(response.data(http::ResponseMode::Cgi));
}

void Response::send(fastcgi::Response response)
{
    responseContext_.responseSender().send(std::move(response.data), std::move(response.errorMsg));
}

bool Response::isSent() const
{
    return responseContext_.responseSender().isSent();
}

void Response::executeTask(std::function<void(const TaskContext& ctx)> task)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

    responseContext_.asioDispatcher().postTask(
            std::move(task),
            [this]
            {
                if (requestProcessorQueue_)
                    requestProcessorQueue_->launch();
            });
}

void Response::makeRequest(
        const std::filesystem::path& socketPath,
        fastcgi::Request request,
        const std::function<void(std::optional<fastcgi::Response>)>& responseHandler,
        const std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

    responseContext_.client().makeRequest(
            socketPath,
            std::move(request),
            [this, responseHandler](std::optional<fastcgi::Response> response)
            {
                if (response)
                    responseHandler(std::move(*response));
                else
                    responseHandler(std::nullopt);
                if (requestProcessorQueue_)
                    requestProcessorQueue_->launch();
            },
            timeout);
}

void Response::makeRequest(
        const std::filesystem::path& socketPath,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& httpResponseHandler,
        const std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

    auto fcgiRequestData = request.toFcgiData(http::FormType::Multipart);
    responseContext_.client().makeRequest(
            socketPath,
            fastcgi::Request{std::move(fcgiRequestData.params), std::move(fcgiRequestData.stdIn)},
            [this, httpResponseHandler](std::optional<fastcgi::Response> response)
            {
                if (response)
                    httpResponseHandler(http::responseFromString(response->data));
                else
                    httpResponseHandler(std::nullopt);
                if (requestProcessorQueue_)
                    requestProcessorQueue_->launch();
            },
            timeout);
}

void Response::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        fastcgi::Request request,
        const std::function<void(std::optional<fastcgi::Response>)>& responseHandler,
        const std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

    responseContext_.client().makeRequest(
            ipAddress,
            port,
            std::move(request),
            [this, responseHandler](std::optional<fastcgi::Response> response)
            {
                if (response)
                    responseHandler(std::move(*response));
                else
                    responseHandler(std::nullopt);
                if (requestProcessorQueue_)
                    requestProcessorQueue_->launch();
            },
            timeout);
}

void Response::makeRequest(
        std::string_view ipAddress,
        uint16_t port,
        const http::Request& request,
        const std::function<void(std::optional<http::ResponseView>)>& httpResponseHandler,
        const std::chrono::milliseconds timeout)
{
    if (requestProcessorQueue_)
        requestProcessorQueue_->stop();

    auto [params, stdIn] = request.toFcgiData(http::FormType::Multipart);
    auto fcgiRequest = fastcgi::Request{std::move(params), std::move(stdIn)};
    responseContext_.client().makeRequest(
            ipAddress,
            port,
            std::move(fcgiRequest),
            [this, httpResponseHandler](std::optional<fastcgi::Response> response)
            {
                if (response)
                    httpResponseHandler(http::responseFromString(response->data));
                else
                    httpResponseHandler(std::nullopt);
                if (requestProcessorQueue_)
                    requestProcessorQueue_->launch();
            },
            timeout);
}

void Response::cancelRequest()
{
    responseContext_.client().disconnect();
}

} // namespace asyncgi
