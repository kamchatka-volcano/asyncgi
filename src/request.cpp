#include <asyncgi/request.h>
#include "requestcontext.h"
#include <asyncgi/types.h>
#include <sfun/string_utils.h>

namespace asyncgi{
namespace str = sfun::string_utils;

Request::Request(std::shared_ptr<detail::RequestContext> context)
    : context_{std::move(context)}
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

const std::vector<std::pair<std::string, std::string>>& Request::fcgiParams() const
{
    return context_->fcgiRequest().params();
}

const std::string& Request::fcgiStdIn() const
{
    return context_->fcgiRequest().stdIn();
}

std::string_view Request::ipAddress() const
{
    return context_->request().ipAddress();
}

std::string_view Request::domainName() const
{
    return context_->request().domainName();
}

std::string_view Request::path() const
{
    return context_->request().path();
}

http::RequestMethod Request::method() const
{
    return context_->request().method();
}

std::string_view Request::query(std::string_view name) const
{
    return context_->request().query(name);
}

bool Request::hasQuery(std::string_view name) const
{
    return context_->request().hasQuery(name);
}

std::string_view Request::cookie(std::string_view name) const
{
    return context_->request().cookie(name);
}

bool Request::hasCookie(std::string_view name) const
{
    return context_->request().hasCookie(name);
}

std::string_view Request::formField(std::string_view name, int index) const
{
    return context_->request().formField(name, index);
}

int Request::formFieldCount(std::string_view name) const
{
    return context_->request().formFieldCount(name);
}

bool Request::hasFormField(std::string_view name) const
{
    return context_->request().hasFormField(name);
}

std::string_view Request::fileData(std::string_view name, int index) const
{
    return context_->request().fileData(name, index);
}

int Request::fileCount(std::string_view name) const
{
    return context_->request().fileCount(name);
}

bool Request::hasFile(std::string_view name) const
{
    return context_->request().hasFile(name);
}

std::string_view Request::fileName(std::string_view name, int index) const
{
    return context_->request().fileName(name, index);
}

std::string_view Request::fileType(std::string_view name, int index) const
{
    return context_->request().fileType(name, index);
}

const std::vector<http::QueryView>& Request::queries() const
{
    return context_->request().queries();
}

const std::vector<http::CookieView>& Request::cookies() const
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



