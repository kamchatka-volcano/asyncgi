#ifndef ASYNCGI_TASKCONTEXT_H
#define ASYNCGI_TASKCONTEXT_H

#include "detail/asio_namespace.h"
#include "detail/external/sfun/member.h"

namespace ASYNCGI_ASIO {
class io_context;
}

namespace asyncgi {

class AsioContext {

public:
    AsioContext(asio::io_context& io);
    asio::io_context& io() const;

private:
    sfun::member<asio::io_context&> io_;
};

} // namespace asyncgi

#endif //ASYNCGI_TASKCONTEXT_H