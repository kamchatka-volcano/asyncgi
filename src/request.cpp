#include <asyncgi/request.h>
#include <asyncgi/types.h>
#include <fcgi_responder/request.h>

namespace asyncgi{

Request::Request(const fcgi::Request& request)
    : fcgiRequest_{request}
    , httpRequest_{[this]{
        return http::RequestView{
            fcgiRequest().hasParam("REQUEST_METHOD") ? fcgiRequest().param("REQUEST_METHOD") : std::string_view{},
            fcgiRequest().hasParam("REMOTE_ADDR") ? fcgiRequest().param("REMOTE_ADDR") : std::string_view{},
            fcgiRequest().hasParam("HTTP_HOST") ? fcgiRequest().param("HTTP_HOST") : std::string_view{},
            fcgiRequest().hasParam("REQUEST_URI") ? fcgiRequest().param("REQUEST_URI") : std::string_view{},
            fcgiRequest().hasParam("QUERY_STRING") ? fcgiRequest().param("QUERY_STRING") : std::string_view{},
            fcgiRequest().hasParam("HTTP_COOKIE") ? fcgiRequest().param("HTTP_COOKIE") : std::string_view{},
            fcgiRequest().hasParam("CONTENT_TYPE") ? fcgiRequest().param("CONTENT_TYPE") : std::string_view{},
            fcgiRequest().stdIn()};
        }}
{
}

const fcgi::Request& Request::fcgiRequest() const
{
    return fcgiRequest_;
}

const http::RequestView& Request::httpRequest() const
{
    return httpRequest_;
}

const std::string& Request::fcgiParam(const std::string &name) const
{
    return fcgiRequest().param(name);
}

bool Request::hasFcgiParam(const std::string &name) const
{
    return fcgiRequest().hasParam(name);
}

const std::vector<std::pair<std::string, std::string>>& Request::fcgiParams() const
{
    return fcgiRequest().params();
}

const std::string& Request::fcgiStdIn() const
{
    return fcgiRequest().stdIn();
}

std::string_view Request::ipAddress() const
{
    return httpRequest().ipAddress();
}

std::string_view Request::domainName() const
{
    return httpRequest().domainName();
}

std::string_view Request::path() const
{
    return httpRequest().path();
}

http::RequestMethod Request::method() const
{
    return httpRequest().method();
}

std::string_view Request::query(std::string_view name) const
{
    return httpRequest().query(name);
}

bool Request::hasQuery(std::string_view name) const
{
    return httpRequest().hasQuery(name);
}

std::string_view Request::cookie(std::string_view name) const
{
    return httpRequest().cookie(name);
}

bool Request::hasCookie(std::string_view name) const
{
    return httpRequest().hasCookie(name);
}

std::string_view Request::formField(std::string_view name, int index) const
{
    return httpRequest().formField(name, index);
}

int Request::formFieldCount(std::string_view name) const
{
    return httpRequest().formFieldCount(name);
}

bool Request::hasFormField(std::string_view name) const
{
    return httpRequest().hasFormField(name);
}

std::string_view Request::fileData(std::string_view name, int index) const
{
    return httpRequest().fileData(name, index);
}

int Request::fileCount(std::string_view name) const
{
    return httpRequest().fileCount(name);
}

bool Request::hasFile(std::string_view name) const
{
    return httpRequest().hasFile(name);
}

std::string_view Request::fileName(std::string_view name, int index) const
{
    return httpRequest().fileName(name, index);
}

std::string_view Request::fileType(std::string_view name, int index) const
{
    return httpRequest().fileType(name, index);
}

const std::vector<http::QueryView>& Request::queries() const
{
    return httpRequest().queries();
}

const std::vector<http::CookieView>& Request::cookies() const
{
    return httpRequest().cookies();
}

std::vector<std::string> Request::formFieldList() const
{
    return httpRequest().formFieldList();
}

std::vector<std::string> Request::fileList() const
{
    return httpRequest().fileList();
}

bool Request::hasFiles() const
{
    return httpRequest().hasFiles();
}

}



