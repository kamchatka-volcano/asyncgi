#pragma once
#include "external/sfun/interface.h"
#include <string>

namespace asyncgi::detail {

class IResponseSender : private sfun::Interface<IResponseSender> {
public:
    virtual void send(std::string data) = 0;
    virtual void send(std::string data, std::string errorMsg) = 0;
    virtual bool isSent() const = 0;
};

} // namespace asyncgi::detail
