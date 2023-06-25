#ifndef ASYNCGI_ERRORS_H
#define ASYNCGI_ERRORS_H

#include <functional>
#include <stdexcept>
#include <system_error>

namespace asyncgi {

struct Error : public std::runtime_error {
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
    explicit ErrorHandler(ErrorHandlerFunc func);
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    ErrorHandler(ErrorHandler&&) = default;
    ErrorHandler& operator=(ErrorHandler&&) = default;
    void operator()(ErrorType, const std::error_code& errorCode);
    void operator()(ErrorType, int code, const std::string& msg);

private:
    ErrorHandlerFunc func_;
};
} // namespace asyncgi

#endif //ASYNCGI_ERRORS_H