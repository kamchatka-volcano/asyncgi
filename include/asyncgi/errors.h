#pragma once
#include <functional>
#include <system_error>

namespace asyncgi{
enum ErrorType{
    ConnectionError,
    SocketReadError,
    SocketWriteError,
    SocketCloseError,
    RequestProcessingError
};

using ErrorHandlerFunc = std::function<void(ErrorType, int code, const std::string& msg)>;

class ErrorHandler{
public:
    ErrorHandler(ErrorHandlerFunc func);
    void operator()(ErrorType, const std::error_code& errorCode);
    void operator()(ErrorType, int code, const std::string& msg);

private:
    ErrorHandlerFunc func_;
};
}
