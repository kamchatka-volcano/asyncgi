#pragma once
#include <fcgi_responder/response.h>

namespace fcgi {
class Response;
}

namespace asyncgi::detail {

class ResponseSender {
public:
    explicit ResponseSender(fcgi::Response response);
    void send(std::string data);
    void send(std::string data, std::string errorMsg);
    bool isSent() const;

private:
    fcgi::Response response_;
};

} // namespace asyncgi::detail
