#pragma once
#include <map>
#include <string>
#include <optional>
#include <filesystem>

namespace asyncgi{
class RequestProcessor;
namespace fs = std::filesystem;

struct TestFormParam{
    struct FileInfo{
        fs::path filePath;
        std::string mimeType;
    };
    TestFormParam(std::string value)
            : value(std::move(value))
    {}
    TestFormParam(std::string fileName, std::string mimeType)
            : fileInfo{FileInfo{std::move(fileName), std::move(mimeType)}}
    {}
    std::optional<std::string> value;
    std::optional<FileInfo> fileInfo;
};

class TestServer{
public:
    TestServer(RequestProcessor& requestProcessor);
    std::string process(const std::map<std::string, std::string>& fcgiParams,
                        const std::map<std::string, TestFormParam>& formParams);

private:
    RequestProcessor& requestProcessor_;
};

}
