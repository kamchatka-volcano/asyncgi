#ifndef ASYNCGI_ASIO_NAMESPACE_H
#define ASYNCGI_ASIO_NAMESPACE_H

#ifdef ASYNCGI_USE_BOOST_ASIO
#define ASYNCGI_ASIO boost::asio
namespace boost::asio {
}
namespace asyncgi {
namespace asio = boost::asio;
}
#else
#define ASYNCGI_ASIO asio
namespace asio {
}
namespace asyncgi {
namespace asio = ::asio;
}
#endif

#endif //ASYNCGI_ASIO_NAMESPACE_H
