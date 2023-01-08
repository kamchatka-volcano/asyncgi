#pragma once
#include <functional>
#include <memory>

namespace asio {
class io_context;
}

namespace asyncgi {

class TaskContext {
    class PostAction {
    public:
        explicit PostAction(std::function<void()> action);
        ~PostAction();

    private:
        std::function<void()> action_;
    };

public:
    TaskContext(asio::io_context& io, std::function<void()> postTaskAction);
    asio::io_context& io() const;

private:
    std::reference_wrapper<asio::io_context> io_;
    std::shared_ptr<PostAction> postTaskAction_;
};

} // namespace asyncgi