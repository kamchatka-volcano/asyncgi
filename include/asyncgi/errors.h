#ifndef ASYNCGI_ERRORS_H
#define ASYNCGI_ERRORS_H
#include <stdexcept>

namespace asyncgi {

class Error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace asyncgi

#endif //ASYNCGI_ERRORS_H