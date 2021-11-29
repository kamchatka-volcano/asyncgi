#pragma once
#include <hot_teacup/cookie.h>
#include <hot_teacup/query.h>
#include <hot_teacup/types.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

namespace asyncgi{

class RequestContext;

class Request{
public:
    const std::string& ipAddress() const;
    std::string domainName() const;
    std::string path() const;
    const std::string& param(const std::string& name) const;
    bool hasParam(const std::string& name) const;
    std::vector<std::string> paramList() const;
    std::vector<std::string> missingFCGIParams() const;

    //http::Request
    http::RequestMethod method() const;
    const http::Queries& queries() const;
    std::vector<std::string> queryList() const;
    const std::string& query(const std::string& name) const;
    bool hasQuery(const std::string& name) const;
    const std::string& cookie(const std::string& name) const;

    const http::Cookies& cookies() const;
    std::vector<std::string> cookieList() const;
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

    Request(std::shared_ptr<RequestContext> context);

private:
    std::shared_ptr<RequestContext> context_;
};

}
