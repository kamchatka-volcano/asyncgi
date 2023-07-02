#ifndef ASYNCGI_ASIODISPATCHER_H
#define ASYNCGI_ASIODISPATCHER_H

#include "taskcontext.h"
#include "detail/serviceholder.h"
#include <functional>
#include <memory>

namespace asyncgi {
namespace detail {
class AsioDispatcherService;
}

class IO;
class Response;

class AsioDispatcher {
public:
    explicit AsioDispatcher(IO&);
    explicit AsioDispatcher(Response&);

    void postTask(std::function<void(const TaskContext&)> task);

private:
    detail::ServiceHolder<detail::AsioDispatcherService> asioDispatcherService_;
};

} //namespace asyncgi

#endif //ASYNCGI_ASIODISPATCHER_H
