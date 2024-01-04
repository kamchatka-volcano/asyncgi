#ifndef ASYNCGI_RESPONDER_H
#define ASYNCGI_RESPONDER_H

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

class Responder {
public:
    explicit Responder(std::shared_ptr<detail::ResponseContext> responseContext);

    template<typename... TArgs>
    void send(TArgs... args)
    {
        auto response = http::Response{std::forward<TArgs>(args)...};
        send(response);
    }

    void send(const http::Response& response);
    void send(fastcgi::Response response);
    bool isSent() const;

    /// detail
    std::weak_ptr<detail::ResponseContext> context(detail::ResponseContextAccessPermission);

private:
    std::weak_ptr<detail::ResponseContext> responseContext_;
};

} // namespace asyncgi

#endif //ASYNCGI_RESPONDER_H