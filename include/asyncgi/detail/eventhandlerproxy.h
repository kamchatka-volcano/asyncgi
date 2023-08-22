#ifndef ASYNCGI_EVENTHANDLERPROXY_H
#define ASYNCGI_EVENTHANDLERPROXY_H
#include "utils.h"
#include "external/sfun/utility.h"
#include <asyncgi/events.h>
#include <functional>
#include <string_view>
#include <type_traits>

namespace asyncgi::detail {

class EventHandlerProxy {
public:
    EventHandlerProxy();
    template<
            typename TEventHandler,
            std::enable_if_t<!std::is_same_v<std::decay_t<TEventHandler>, EventHandlerProxy>>* = nullptr>
    EventHandlerProxy(TEventHandler&& eventHandler)
    {
        if constexpr (std::is_invocable_v<TEventHandler, ErrorEvent, std::string_view>)
            errorHandler_ = refWrapperOrRValue(std::forward<TEventHandler>(eventHandler));
        else
            static_assert(sfun::dependent_false<TEventHandler>, "TEventHandler has an incompatible signature");
    }

    void operator()(ErrorEvent, std::string_view);

private:
    std::function<void(ErrorEvent, std::string_view)> errorHandler_ = [](ErrorEvent, std::string_view) {};
};

} //namespace asyncgi::detail

#endif //ASYNCGI_EVENTHANDLERPROXY_H
