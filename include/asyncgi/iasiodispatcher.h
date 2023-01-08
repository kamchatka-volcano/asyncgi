#pragma once
#include "taskcontext.h"
#include "detail/external/sfun/interface.h"
#include <functional>
#include <memory>

namespace asyncgi {

class IAsioDispatcher : private sfun::Interface<IAsioDispatcher> {
public:
    virtual void postTask(std::function<void(const TaskContext& ctx)> task, std::function<void()> postTaskAction) = 0;
    virtual void postTask(std::function<void(const TaskContext& ctx)> task) = 0;
};

} // namespace asyncgi