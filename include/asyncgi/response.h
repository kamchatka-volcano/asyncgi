#ifndef ASYNCGI_RESPONSE_H
#define ASYNCGI_RESPONSE_H

#include "types.h"
#include "detail/external/sfun/interface.h"
#include "http/request.h"
#include "http/response.h"
#include <filesystem>
#include <functional>
#include <future>
#include <optional>

namespace asyncgi {

namespace whaleroute {
class RequestProcessorQueue;
}

template<typename TContext>
class Router;
class Client;
class Timer;
class AsioDispatcher;

namespace detail {
class ResponseContext;
struct RouterResponseContextAccessor;
using ResponseContextAccessPermission =
        sfun::access_permission<Client, Timer, AsioDispatcher, RouterResponseContextAccessor>;
} //namespace detail

class Response {
public:
    explicit Response(detail::ResponseContext& responseContext);

    template<typename... TArgs>
    void send(TArgs... args)
    {
        auto response = http::Response{std::forward<TArgs>(args)...};
        send(response);
    }

    void redirect(
            std::string path,
            http::RedirectType redirectType = http::RedirectType::Found,
            std::vector<http::Cookie> cookies = {},
            std::vector<http::Header> headers = {});
    void send(const http::Response& response);
    void send(fastcgi::Response response);
    bool isSent() const;

    /// detail
    detail::ResponseContext& context(detail::ResponseContextAccessPermission);

private:
    sfun::member<detail::ResponseContext&> responseContext_;
};

} // namespace asyncgi

#endif //ASYNCGI_RESPONSE_H