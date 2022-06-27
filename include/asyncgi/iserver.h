#pragma once
#include <filesystem>

namespace asyncgi{
class IServer {
public:
    IServer() = default;
    virtual ~IServer() = default;
    IServer(const IServer&) = delete;
    IServer& operator=(const IServer&) = delete;
    IServer(IServer&&) = delete;
    IServer&& operator=(IServer&&) = delete;

    virtual void listen(const std::filesystem::path& socketPath) = 0;
};

}