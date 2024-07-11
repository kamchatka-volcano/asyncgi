#ifndef ASYNCGI_FASTCGI_REQUEST_H
#define ASYNCGI_FASTCGI_REQUEST_H

#include <asyncgi/detail/external/sfun/member.h>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace asyncgi::detail::fcgi {
class Request;
}

namespace asyncgi::fastcgi {
class Request;

namespace detail {
class RequestView {
public:
    explicit RequestView(const asyncgi::detail::fcgi::Request&);
    std::string_view stdIn() const;
    std::string_view param(const std::string& name) const;

    const std::vector<std::pair<std::string, std::string>>& params() const;
    bool hasParam(const std::string& name) const;

private:
    sfun::member<const asyncgi::detail::fcgi::Request&> fcgiRequest_;
};

struct RequestDataParams {
    std::vector<std::pair<std::string, std::string>> params;
    std::string stdIn;
};

RequestDataParams moveOutRequestData(Request&);

class RequestData {
public:
    RequestData(std::map<std::string, std::string> params, std::string stdIn);
    std::string_view stdIn() const;
    std::string_view param(const std::string& name) const;
    bool hasParam(const std::string& name) const;
    const std::vector<std::pair<std::string, std::string>>& params() const;

private:
    RequestDataParams paramData_;

    friend detail::RequestDataParams detail::moveOutRequestData(Request&);
};

} //namespace detail

class Request {
public:
    explicit Request(detail::RequestView);
    Request(std::map<std::string, std::string> params, std::string stdIn);

    std::string_view stdIn() const;
    std::string_view param(const std::string& name) const;
    bool hasParam(const std::string& name) const;
    const std::vector<std::pair<std::string, std::string>>& params() const;

private:
    bool isView() const;

private:
    std::variant<detail::RequestData, detail::RequestView> data_;

    friend detail::RequestDataParams detail::moveOutRequestData(Request&);
};

} //namespace asyncgi::fastcgi

#endif //ASYNCGI_FAST_REQUEST_H
