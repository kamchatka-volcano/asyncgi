#ifndef ASYNCGI_ASIO_ERROR_H
#define ASYNCGI_ASIO_ERROR_H

#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/error.hpp>
namespace asyncgi {
using asio_error = boost::system::error_code;
}
#else
#include <asio/error_code.hpp>
namespace asyncgi {
using asio_error = std::error_code;
}
#endif

#endif //ASYNCGI_ASIO_ERRORCODE_H
