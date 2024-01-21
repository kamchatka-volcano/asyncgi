#ifndef ASYNCGI_FWD_H
#define ASYNCGI_FWD_H

namespace asyncgi {
class IO;
class Request;
class Responder;
class Server;
class Client;
class Error;
class AsioDispatcher;
class TaskContext;
class Timer;

namespace http {
class Response;
};
namespace fastcgi {
struct Request;
struct Response;
} //namespace fastcgi

} //namespace asyncgi

#endif //ASYNCGI_FWD_H