#pragma once
#include <filesystem>

namespace asyncgi{
class IServer {
public:
    virtual ~IServer() = default;
    virtual void listen(const std::filesystem::path& socketPath) = 0;
};

}