#pragma once
#include <string>

namespace asyncgi::detail{

class IResponseSender{
public:
    IResponseSender() = default;
    virtual ~IResponseSender() = default;
    IResponseSender(const IResponseSender&) = delete;
    IResponseSender& operator=(const IResponseSender&) = delete;
    IResponseSender(IResponseSender&&) = delete;
    IResponseSender&& operator=(IResponseSender&&) = delete;

    virtual void send(std::string data) = 0;
    virtual void send(std::string data, std::string errorMsg) = 0;
    virtual bool isSent() const = 0;
};

}
