#ifndef ASYNCGI_REQUEST_H
#define ASYNCGI_REQUEST_H

#include "detail/external/sfun/member.h"
#include "detail/lazyinitialized.h"
#include "http/cookie_view.h"
#include "http/query_view.h"
#include "http/request_view.h"
#include "http/types.h"
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace asyncgi {

namespace fcgi {
class Request;
}

class Request {
public:
    // as http::Request
    std::string_view ipAddress() const;
    std::string_view domainName() const;
    std::string_view path() const;
    http::RequestMethod method() const;
    const std::vector<http::QueryView>& queries() const;
    std::string_view query(std::string_view name) const;
    bool hasQuery(std::string_view name) const;

    const std::vector<http::CookieView>& cookies() const;
    std::string_view cookie(std::string_view name) const;
    bool hasCookie(std::string_view name) const;

    std::string_view formField(std::string_view name, int index = 0) const;
    std::vector<std::string> formFieldList() const;
    std::vector<std::string> fileList() const;
    int formFieldCount(std::string_view name) const;
    bool hasFormField(std::string_view name) const;

    std::string_view fileData(std::string_view name, int index = 0) const;
    int fileCount(std::string_view name) const;
    bool hasFile(std::string_view name) const;
    std::string_view fileName(std::string_view name, int index = 0) const;
    std::string_view fileType(std::string_view name, int index = 0) const;
    bool hasFiles() const;

    // as FCGIRequest
    const std::string& fcgiParam(const std::string& name) const;
    bool hasFcgiParam(const std::string& name) const;
    const std::vector<std::pair<std::string, std::string>>& fcgiParams() const;
    const std::string& fcgiStdIn() const;

    explicit Request(const fcgi::Request& fcgiRequest);

private:
    const fcgi::Request& fcgiRequest() const;
    const http::RequestView& httpRequest() const;

private:
    sfun::member<const fcgi::Request&> fcgiRequest_;
    detail::LazyInitialized<http::RequestView> httpRequest_;
};

} // namespace asyncgi

#endif //ASYNCGI_REQUEST_H