#ifndef ASYNCGI_FWD_H
#define ASYNCGI_FWD_H

namespace asyncgi {
class IO;
class Responder;
class Server;
class FastCgiClient;
class Error;
class AsioDispatcher;
class AsioContext;
class Timer;
#ifdef ASYNCGI_USE_HTTP_CLIENT
class HttpClient;
#endif

namespace http {
class Request;
class Response;
}; //namespace http

namespace fastcgi {
class Request;
class Response;
} //namespace fastcgi

} //namespace asyncgi

#ifndef ASYNCGI_DISABLE_GLOBAL_HTTP_NAMESPACE
namespace http = asyncgi::http;
#endif

#ifndef ASYNCGI_DISABLE_GLOBAL_FASTCGI_NAMESPACE
namespace fastcgi = asyncgi::fastcgi;
#endif

#endif //ASYNCGI_FWD_H