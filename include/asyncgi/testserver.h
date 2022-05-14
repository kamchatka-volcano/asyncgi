#pragma once
#include <map>
#include <string>
#include <optional>
#include <variant>
#include <filesystem>

namespace asyncgi{
namespace detail {
class IRequestProcessor;
}
namespace fs = std::filesystem;

struct TestFormFile{
    fs::path filePath;
    std::optional<std::string> mimeType;
};

struct TestFormParam{
    std::variant<std::string, TestFormFile> value;
};

class TestServer{
public:
    TestServer(detail::IRequestProcessor& requestProcessor);
    std::string process(const std::map<std::string, std::string>& fcgiParams,
                        const std::map<std::string, TestFormParam>& formParams);

private:
    detail::IRequestProcessor& requestProcessor_;
};

}
