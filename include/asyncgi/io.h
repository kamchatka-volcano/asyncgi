#ifndef ASYNCGI_IO_H
#define ASYNCGI_IO_H

#include "errors.h"
#include "detail/eventhandlerproxy.h"
#include "detail/external/sfun/interface.h"
#include <memory>

namespace asyncgi {
class Server;
class Client;
class Timer;
class AsioDispatcher;
class RouterIOAccess;

namespace detail {
class IOService;
using IOAccessPermission = sfun::access_permission<Server, Client, Timer, AsioDispatcher, RouterIOAccess>;
} //namespace detail

class IO {

public:
    template<typename TEventHandler>
    explicit IO(int threadsNumber, TEventHandler&& eventHandler)
        : IO(threadsNumber)
    {
        eventHandler_ = std::forward<TEventHandler>(eventHandler);
    }

    template<typename TEventHandler>
    explicit IO(TEventHandler&& eventHandler)
        : IO(1)
    {
        eventHandler_ = std::forward<TEventHandler>(eventHandler);
    }
    explicit IO(int threadsNumber = 1);

    ~IO();
    IO(const IO&) = delete;
    IO& operator=(const IO&) = delete;
    IO(IO&&) = delete;
    IO& operator=(IO&&) = delete;

    void run();
    void stop();

    /// detail
    detail::EventHandlerProxy& eventHandler(detail::IOAccessPermission);
    detail::IOService& ioService(detail::IOAccessPermission);

private:
    std::unique_ptr<detail::IOService> ioService_;
    detail::EventHandlerProxy eventHandler_;
};

} //namespace asyncgi

#endif //ASYNCGI_IO_H
