#ifndef ASYNCGI_ASIODISPATCHER_H
#define ASYNCGI_ASIODISPATCHER_H

#include "taskcontext.h"
#include <functional>
#include <memory>

namespace asyncgi {
namespace detail {
class AsioDispatcherService;
}

class IO;

class AsioDispatcher {
public:
    AsioDispatcher(IO&);
    ~AsioDispatcher();
    AsioDispatcher(const AsioDispatcher&) = delete;
    AsioDispatcher(AsioDispatcher&&) = default;
    AsioDispatcher& operator=(const AsioDispatcher&) = delete;
    AsioDispatcher& operator=(AsioDispatcher&&) = default;

    void postTask(std::function<void(const TaskContext&)> task, std::function<void()> postTaskAction);
    void postTask(std::function<void(const TaskContext&)> task);

private:
    std::unique_ptr<detail::AsioDispatcherService> asioDispatcherService_;
};

} //namespace asyncgi

#endif //ASYNCGI_ASIODISPATCHER_H
