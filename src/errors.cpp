#include <asyncgi/errors.h>

namespace asyncgi {

ErrorHandler::ErrorHandler(ErrorHandlerFunc func)
    : func_{std::move(func)}
{
}

void ErrorHandler::operator()(ErrorType errorType, const std::error_code& errorCode)
{
    const auto lock = std::scoped_lock{mutex_};
    if (func_)
        func_(errorType, errorCode.value(), errorCode.message());
}

void ErrorHandler::operator()(ErrorType errorType, int code, const std::string& msg)
{
    const auto lock = std::scoped_lock{mutex_};
    if (func_)
        func_(errorType, code, msg);
}

} // namespace asyncgi
