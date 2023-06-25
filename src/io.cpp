#include "ioservice.h"
#include <asyncgi/io.h>

namespace asyncgi {

IO::IO(int threadsNumber, ErrorHandlerFunc errorHandler)
    : ioService_{std::make_unique<detail::IOService>(threadsNumber)}
    , errorHandler_{std::move(errorHandler)}
{
}

IO::~IO() = default;

detail::IOService& IO::ioService(detail::AccessPermission)
{
    return *ioService_;
}

ErrorHandler& IO::errorHandler()
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