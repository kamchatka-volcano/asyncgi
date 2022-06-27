#pragma once
#include "types.h"
#include <hot_teacup/request.h>
#include <hot_teacup/response.h>
#include <filesystem>
#include <map>
#include <string>
#include <optional>
#include <functional>
#include <chrono>

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
            const std::filesystem::path& socketPath,
            std::map<std::string, std::string> fcgiParams,
            std::string fcgiStdIn,
            std::function<void(const std::optional<std::string>&)> responseHandler,
            const std::chrono::milliseconds& timeout = std::chrono::seconds{3}) = 0;
    virtual void makeRequest(
            const std::filesystem::path& socketPath,
            const http::Request& request,
            const std::function<void(const std::optional<http::Response>&)>& responseHandler,
            const std::chrono::milliseconds& timeout = std::chrono::seconds{3}) = 0;
    virtual void disconnect() = 0;
};

}