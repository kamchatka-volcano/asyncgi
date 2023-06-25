#ifndef ASYNCGI_TESTSERVER_H
#define ASYNCGI_TESTSERVER_H

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <variant>

namespace asyncgi {
namespace http {
class Request;
}
namespace detail {
class IRequestProcessor;
}
namespace fs = std::filesystem;

class TestServer {
public:
    explicit TestServer(detail::IRequestProcessor& requestProcessor);
    std::string process(const std::map<std::string, std::string>& fcgiParams, const std::string& fcgiData);
    std::string process(const http::Request&);

private:
    detail::IRequestProcessor& requestProcessor_;
};

} // namespace asyncgi

#endif //ASYNCGI_TESTSERVER_H