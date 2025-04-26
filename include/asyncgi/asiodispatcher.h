#ifndef ASYNCGI_ASIODISPATCHER_H
#define ASYNCGI_ASIODISPATCHER_H

#include "asyncgi_fwd.h"
#include "detail/serviceholder.h"
#include <functional>
#include <memory>

namespace asyncgi {
namespace detail {
class AsioDispatcherService;
}

class AsioDispatcher {
public:
    explicit AsioDispatcher(IO&);
    explicit AsioDispatcher(Responder&);

    void postTask(std::function<void(const AsioContext&)> task);

private:
    detail::ServiceHolder<detail::AsioDispatcherService> asioDispatcherService_;
};

} //namespace asyncgi

#endif //ASYNCGI_ASIODISPATCHER_H
