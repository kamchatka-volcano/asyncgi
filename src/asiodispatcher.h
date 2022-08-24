#pragma once
#include <asyncgi/iasiodispatcher.h>

namespace asio{
    class io_context;
}

namespace asyncgi::detail{

class AsioDispatcher : public IAsioDispatcher{
public:
    explicit AsioDispatcher(asio::io_context& io);
    void postTask(std::function<void(const TaskContext&)> task, std::function<void()> postTaskAction) override;
    void postTask(std::function<void(const TaskContext&)> task) override;

private:
    asio::io_context& io_;
};

}
