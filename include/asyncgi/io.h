#ifndef ASYNCGI_IO_H
#define ASYNCGI_IO_H

#include "errors.h"
#include "detail/external/sfun/interface.h"
#include <memory>

namespace asyncgi {
class Server;
class Client;
class Timer;
class AsioDispatcher;

namespace detail {
class IOService;
using IOAccessPermission = sfun::access_permission<Server, Client, Timer, AsioDispatcher>;
} //namespace detail

class IO {

public:
    explicit IO(int threadsNumber = 1, ErrorHandlerFunc = {});
    ~IO();
    IO(const IO&) = delete;
    IO(IO&&) = default;
    IO& operator=(const IO&) = delete;
    IO& operator=(IO&&) = delete;

    void run();
    void stop();

    /// detail
    ErrorHandler& errorHandler(detail::IOAccessPermission);
    detail::IOService& ioService(detail::IOAccessPermission);

private:
    std::unique_ptr<detail::IOService> ioService_;
    ErrorHandler errorHandler_;
};

} //namespace asyncgi

#endif //ASYNCGI_IO_H
