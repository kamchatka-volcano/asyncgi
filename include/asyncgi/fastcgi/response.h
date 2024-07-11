#ifndef ASYNCGI_FASTCGI_RESPONSE_H
#define ASYNCGI_FASTCGI_RESPONSE_H
#include <string>
#include <variant>

namespace asyncgi::fastcgi {
class Response;

namespace detail {

struct ResponseView {
    std::string_view data;
    std::string_view errorMsg;
};

struct ResponseData {
    std::string data;
    std::string errorMsg;
};
ResponseData moveOutResponseData(Response&);
} //namespace detail

class Response {
public:
    explicit Response(detail::ResponseView);
    Response(std::string data, std::string errorMsg = {});
    std::string_view data() const;
    std::string_view errorMsg() const;

private:
    bool isView() const;

private:
    std::variant<detail::ResponseData, detail::ResponseView> data_;
    friend detail::ResponseData detail::moveOutResponseData(Response&);
};

} //namespace asyncgi::fastcgi

#endif //ASYNCGI_RESPONSE_H
