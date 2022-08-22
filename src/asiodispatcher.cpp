#include "asiodispatcher.h"
#include <asio/io_context.hpp>

namespace asyncgi::detail{

AsioDispatcher::AsioDispatcher(asio::io_context& io)
    : io_{io}
{}

void AsioDispatcher::dispatch(std::function<void(asio::io_context&)> task)
{
    io_.dispatch([this, task = std::move(task)]{task(io_);});
}

}