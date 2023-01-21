#pragma once
#include <functional>
#include <system_error>
#include <stdexcept>

namespace asyncgi {

struct Error : public std::runtime_error{
    using std::runtime_error::runtime_error;
};

enum ErrorType {
    ConnectionError,
    SocketReadError,
    SocketWriteError,
    SocketCloseError,
    RequestProcessingError
};

using ErrorHandlerFunc = std::function<void(ErrorType, int code, const std::string& msg)>;

class ErrorHandler {
public:
    ErrorHandler(ErrorHandlerFunc func);
    void operator()(ErrorType, const std::error_code& errorCode);
    void operator()(ErrorType, int code, const std::string& msg);

private:
    ErrorHandlerFunc func_;
};
} // namespace asyncgi
