#include <asyncgi/asio_context.h>
#include <asyncgi/detail/asio_namespace.h>

namespace asyncgi {

AsioContext::AsioContext(asio::io_context& io)
    : io_{io}
{
}

asio::io_context& AsioContext::io() const
{
    return io_;
}

} // namespace asyncgi
