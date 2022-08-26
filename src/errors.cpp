#include <asyncgi/errors.h>

namespace asyncgi{

ErrorHandler::ErrorHandler(ErrorHandlerFunc func)
    : func_{std::move(func)}
{}

void ErrorHandler::operator()(ErrorType errorType, const std::error_code& errorCode)
{
    if (func_)
        func_(errorType, errorCode.value(), errorCode.message());
}

void ErrorHandler::operator()(ErrorType errorType, int code, const std::string& msg)
{
    if (func_)
        func_(errorType, code, msg);
}

}

