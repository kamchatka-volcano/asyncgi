#pragma once
#include "types.h"
#include <hot_teacup/request.h>
#include <hot_teacup/response_view.h>
#include <filesystem>
#include <string_view>
#include <map>
#include <string>
#include <optional>
#include <functional>
#include <chrono>
#include <cstdint>

namespace asyncgi{

class IClient{
public:
    IClient() = default;
    virtual ~IClient() = default;
    IClient(const IClient&) = delete;
    IClient& operator=(const IClient&) = delete;
    IClient(IClient&&) = delete;
    IClient&& operator=(IClient&&) = delete;

    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            fastcgi::Request request,
            std::function<void(const std::optional<fastcgi::Response>&)> responseHandler) = 0;
    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            fastcgi::Request request,
            std::function<void(const std::optional<fastcgi::Response>&)> responseHandler,
            const std::chrono::milliseconds& timeout) = 0;
    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            const http::Request& request,
            const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler) = 0;
    virtual void makeRequest(
            const std::filesystem::path& unixDomainSocketPath,
            const http::Request& request,
            const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler,
            const std::chrono::milliseconds& timeout) = 0;

     virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request,
            std::function<void(const std::optional<fastcgi::Response>&)> responseHandler) = 0;
    virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            fastcgi::Request,
            std::function<void(const std::optional<fastcgi::Response>&)> responseHandler,
            const std::chrono::milliseconds& timeout) = 0;
    virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler) = 0;
    virtual void makeRequest(
            std::string_view ipAddress,
            uint16_t port,
            const http::Request& request,
            const std::function<void(const std::optional<http::ResponseView>&)>& responseHandler,
            const std::chrono::milliseconds& timeout) = 0;

    virtual void disconnect() = 0;
};

}