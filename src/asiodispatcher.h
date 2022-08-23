#pragma once
#include <asyncgi/iasiodispatcher.h>

namespace asio{
    class io_context;
}

namespace asyncgi::detail{

class AsioDispatcher : public IAsioDispatcher{
public:
    AsioDispatcher(asio::io_context& io);
    void postTask(std::function<void(const TaskContext&)> task, std::function<void()> postTaskAction) override;

private:
    asio::io_context& io_;
};

}
