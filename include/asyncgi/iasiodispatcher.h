#pragma once
#include "taskcontext.h"
#include<functional>
#include <memory>


namespace asyncgi{

class IAsioDispatcher{
public:
    IAsioDispatcher() = default;
    virtual ~IAsioDispatcher() = default;
    IAsioDispatcher(const IAsioDispatcher&) = delete;
    IAsioDispatcher& operator=(const IAsioDispatcher&) = delete;
    IAsioDispatcher(IAsioDispatcher&&) = delete;
    IAsioDispatcher&& operator=(IAsioDispatcher&&) = delete;

    virtual void postTask(std::function<void(const TaskContext& ctx)> task, std::function<void()> postTaskAction) = 0;
    virtual void postTask(std::function<void(const TaskContext& ctx)> task) = 0;
};

}