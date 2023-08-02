#include "ioservice.h"
#include <asyncgi/detail/external/sfun/utility.h>
#include <asyncgi/io.h>

namespace asyncgi {

IO::IO(int threadsNumber, ErrorHandlerFunc errorHandler)
    : ioService_{std::make_unique<detail::IOService>(threadsNumber)}
    , errorHandler_{std::move(errorHandler)}
{
}

IO::IO(ErrorHandlerFunc errorHandler)
    : ioService_{std::make_unique<detail::IOService>(1)}
    , errorHandler_{std::move(errorHandler)}
{
}

IO::~IO() = default;

detail::IOService& IO::ioService(detail::IOAccessPermission)
{
    return *ioService_;
}

ErrorHandler& IO::errorHandler(detail::IOAccessPermission)
{
    return errorHandler_;
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