#include <asyncgi/detail/eventhandlerproxy.h>

namespace asyncgi::detail {

EventHandlerProxy::EventHandlerProxy() = default;

void EventHandlerProxy::operator()(ErrorEvent event, std::string_view message)
{
    errorHandler_(event, message);
}

} //namespace asyncgi::detail
