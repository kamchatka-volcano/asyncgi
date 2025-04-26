#ifndef ASYNCGI_HTTP_CLIENT_H
#define ASYNCGI_HTTP_CLIENT_H

#ifdef ASYNCGI_USE_HTTP_CLIENT

#include "asyncgi_fwd.h"
#include "detail/serviceholder.h"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>

namespace asyncgi {

namespace detail {
class AsioDispatcherService;
}

class HttpClient {
public:
    explicit HttpClient(IO&);
    explicit HttpClient(Responder&);

    void makeRequest(
            const http::Request& request,
            const std::function<void(std::optional<http::Response>)>& responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{21});
    const std::string& requestErrorMessage() const;

private:
    detail::ServiceHolder<detail::AsioDispatcherService> asioDispatcherService_;
    std::shared_ptr<std::string> requestErrorMessage_;
};

} //namespace asyncgi

#endif //ASYNCGI_USE_HTTP_CLIENT

#endif //ASYNCGI_HTTP_CLIENT_H