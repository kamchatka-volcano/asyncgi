#ifndef ASYNCGI_TASKCONTEXT_H
#define ASYNCGI_TASKCONTEXT_H
#include "detail/asio_namespace.h"
#include "detail/external/sfun/member.h"
#include <functional>
#include <memory>

namespace ASYNCGI_ASIO {
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
    sfun::member<asio::io_context&> io_;
    std::shared_ptr<PostAction> postTaskAction_;
};

} // namespace asyncgi

#endif //ASYNCGI_TASKCONTEXT_H