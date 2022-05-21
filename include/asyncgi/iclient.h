#pragma once
#include "types.h"
#include <hot_teacup/request.h>
#include <hot_teacup/response.h>
#include <filesystem>
#include <map>
#include <string>
#include <optional>
#include <functional>

namespace asyncgi{

class IClient{
public:
    virtual ~IClient() = default;
    virtual void makeRequest(const std::filesystem::path& socketPath,
                             const FCGIRequest& fcgiRequest,
                             const std::function<void(const std::optional<FCGIResponse>&)>& responseHandler) = 0;
    virtual void makeRequest(const std::filesystem::path& socketPath,
                             const http::Request& request,
                             const std::function<void(const std::optional<http::Response>&)>& responseHandler) = 0;
    virtual void disconnect() = 0;
};

}