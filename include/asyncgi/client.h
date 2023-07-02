#ifndef ASYNCGI_CLIENT_H
#define ASYNCGI_CLIENT_H

#include "errors.h"
#include "types.h"
#include "detail/serviceholder.h"
#include "http/request.h"
#include "http/response_view.h"
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

namespace asyncgi {
class IO;
class Response;

namespace detail {
class ClientService;
}

class Client {
public:
    explicit Client(IO&);
    explicit Client(Response&);

    void makeRequest(
            const std::filesystem::path& socketPath,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});
    void makeRequest(
            const std::filesystem::path& socketPath,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});
    void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            std::chrono::milliseconds timeout = std::chrono::seconds{3});

    void disconnect();

private:
    detail::ServiceHolder<detail::ClientService> clientService_;
};

} //namespace asyncgi

#endif //ASYNCGI_CLIENT_H
