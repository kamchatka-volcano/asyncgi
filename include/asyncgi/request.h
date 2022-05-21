#pragma once
#include <hot_teacup/cookie.h>
#include <hot_teacup/query.h>
#include <hot_teacup/types.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

namespace asyncgi{
namespace detail {
class RequestContext;
}

class Request{
public:
    //as http::Request
    const std::string& ipAddress() const;
    const std::string& domainName() const;
    const std::string& path() const;
    http::RequestMethod method() const;
    const http::Queries& queries() const;
    const std::string& query(const std::string& name) const;
    bool hasQuery(const std::string& name) const;
    const std::string& cookie(const std::string& name) const;

    const http::Cookies& cookies() const;
    bool hasCookie(const std::string& name) const;
    const std::string& formField(const std::string &name, int index = 0) const;

    std::vector<std::string> formFieldList() const;
    std::vector<std::string> fileList() const;
    int formFieldCount(const std::string& name) const;
    bool hasFormField(const std::string &name) const;

    const std::string& fileData(const std::string &name, int index = 0) const;
    int fileCount(const std::string &name) const;
    bool hasFile(const std::string &name) const;
    const std::string& fileName(const std::string &name, int index = 0) const;
    const std::string& fileType(const std::string &name, int index = 0) const;
    bool hasFiles() const;

    //as FCGIRequest
    const std::string& fcgiParam(const std::string& name) const;
    bool hasFcgiParam(const std::string& name) const;
    std::vector<std::string> fcgiParamList() const;
    const std::string& fcgiStdIn() const;
    std::map<std::string, std::string> fcgiParamMap() const;

    explicit Request(std::shared_ptr<detail::RequestContext> context);

private:
    std::shared_ptr<detail::RequestContext> context_;
};

}
