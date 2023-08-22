#include "ioservice.h"
#include <asyncgi/io.h>

namespace asyncgi {

IO::IO(int threadsNumber)
    : ioService_{std::make_unique<detail::IOService>(threadsNumber)}
{
}

IO::~IO() = default;

detail::IOService& IO::ioService(detail::IOAccessPermission)
{
    return *ioService_;
}

detail::EventHandlerProxy& IO::eventHandler(detail::IOAccessPermission)
{
    return eventHandler_;
}

void IO::run()
{
    ioService_->run();
}

void IO::stop()
{
    ioService_->stop();
}

} //namespace asyncgi