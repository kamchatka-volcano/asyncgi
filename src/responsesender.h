#pragma once
#include "asyncgi/detail/iresponsesender.h"
#include <fcgi_responder/response.h>

namespace fcgi {
class Response;
}

namespace asyncgi::detail {

class ResponseSender : public IResponseSender {
public:
    explicit ResponseSender(fcgi::Response response);
    void send(std::string data) override;
    void send(std::string data, std::string errorMsg) override;
    bool isSent() const override;

private:
    fcgi::Response response_;
};

} // namespace asyncgi::detail
