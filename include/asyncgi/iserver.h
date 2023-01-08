#pragma once
#include "detail/external/sfun/interface.h"
#include <cstdint>
#include <filesystem>
#include <string_view>

namespace asyncgi {
class IServer : private sfun::Interface<IServer> {
public:
    virtual void listen(const std::filesystem::path& socketPath) = 0;
    virtual void listen(std::string_view ipAddress, uint16_t portNumber) = 0;
};

} // namespace asyncgi