#include <asyncgi/request.h>
#include "requestcontext.h"
#include <asyncgi/types.h>
#include <sfun/string_utils.h>

namespace asyncgi{
namespace str = sfun::string_utils;

Request::Request(std::shared_ptr<detail::RequestContext> context)
    : context_(std::move(context))
{
}

const std::string& Request::fcgiParam(const std::string &name) const
{
    return context_->fcgiRequest().param(name);
}

bool Request::hasFcgiParam(const std::string &name) const
{
    return context_->fcgiRequest().hasParam(name);
}

std::vector<std::string> Request::fcgiParamList() const
{
    return context_->fcgiRequest().paramList();
}

const std::string& Request::fcgiStdIn() const
{
    return context_->fcgiRequest().stdIn();
}

std::map<std::string, std::string> Request::fcgiParamMap() const
{
    return context_->fcgiRequest().paramMap();
}

const std::string& Request::ipAddress() const
{
    return context_->request().ipAddress();
}

const std::string& Request::domainName() const
{
    return context_->request().domainName();
}

const std::string& Request::path() const
{
    return context_->request().path();
}

http::RequestMethod Request::method() const
{
    return context_->request().method();
}

const std::string& Request::query(const std::string& name) const
{
    return context_->request().query(name);
}

bool Request::hasQuery(const std::string& name) const
{
    return context_->request().hasQuery(name);
}

const std::string& Request::cookie(const std::string& name) const
{
    return context_->request().cookie(name);
}

bool Request::hasCookie(const std::string& name) const
{
    return context_->request().hasCookie(name);
}

const std::string& Request::formField(const std::string& name, int index) const
{
    return context_->request().formField(name, index);
}

int Request::formFieldCount(const std::string& name) const
{
    return context_->request().formFieldCount(name);
}

bool Request::hasFormField(const std::string& name) const
{
    return context_->request().hasFormField(name);
}

const std::string& Request::fileData(const std::string& name, int index) const
{
    return context_->request().fileData(name, index);
}

int Request::fileCount(const std::string& name) const
{
    return context_->request().fileCount(name);
}

bool Request::hasFile(const std::string& name) const
{
    return context_->request().hasFile(name);
}

const std::string& Request::fileName(const std::string &name, int index) const
{
    return context_->request().fileName(name, index);
}

const std::string& Request::fileType(const std::string &name, int index) const
{
    return context_->request().fileType(name, index);
}

const http::Queries& Request::queries() const
{
    return context_->request().queries();
}

const http::Cookies& Request::cookies() const
{
    return context_->request().cookies();
}

std::vector<std::string> Request::formFieldList() const
{
    return context_->request().formFieldList();
}

std::vector<std::string> Request::fileList() const
{
    return context_->request().fileList();
}

bool Request::hasFiles() const
{
    return context_->request().hasFiles();
}


}



