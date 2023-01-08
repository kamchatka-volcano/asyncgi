#pragma once
#include "types.h"
#include "detail/external/sfun/interface.h"
#include "http/request.h"
#include "http/response_view.h"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>

namespace asyncgi {

class IClient : private sfun::Interface<IClient> {
public:
    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler) = 0;
    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            fastcgi::Request request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            const std::chrono::milliseconds& timeout) = 0;
    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler) = 0;
    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            const std::chrono::milliseconds& timeout) = 0;

    virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler) = 0;
    virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request,
            std::function<void(std::optional<fastcgi::Response>)> responseHandler,
            const std::chrono::milliseconds& timeout) = 0;
    virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler) = 0;
    virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(std::optional<http::ResponseView>)>& responseHandler,
            const std::chrono::milliseconds& timeout) = 0;

    virtual void disconnect() = 0;
};

} // namespace asyncgi