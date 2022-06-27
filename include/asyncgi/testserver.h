#pragma once
#include <map>
#include <string>
#include <optional>
#include <variant>
#include <filesystem>

namespace http{
class Request;
}

namespace asyncgi{
namespace detail {
class IRequestProcessor;
}
namespace fs = std::filesystem;

class TestServer{
public:
    explicit TestServer(detail::IRequestProcessor& requestProcessor);
    std::string process(const std::map<std::string, std::string>& fcgiParams,
                        const std::string& fcgiData);
    std::string process(const http::Request&);

private:
    detail::IRequestProcessor& requestProcessor_;
};

}
