#ifndef ASYNCGI_REQUESTPROXY_H
#define ASYNCGI_REQUESTPROXY_H

#include "asyncgi/detail/external/sfun/member.h"
#include "asyncgi/detail/lazyinitialized.h"
#include "asyncgi/fastcgi/request.h"
#include "asyncgi/http/cookie_view.h"
#include "asyncgi/http/query_view.h"
#include "asyncgi/http/request.h"
#include "asyncgi/http/types.h"
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace asyncgi::fcgi {
class Request;
}

namespace asyncgi::detail {
class RequestProxy {
public:
    explicit RequestProxy(const fcgi::Request& fcgiRequest);
    operator const fastcgi::Request&() const;
    operator const http::Request&() const;

private:
    fastcgi::Request fcgiRequest_;
    detail::LazyInitialized<http::Request> httpRequest_;
};

} //namespace asyncgi::detail

#endif //ASYNCGI_REQUESTPROXY_H