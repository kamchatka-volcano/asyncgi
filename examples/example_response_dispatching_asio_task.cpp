#include <asyncgi/asyncgi.h>
#include <asio/steady_timer.hpp>

using namespace asyncgi;

struct DelayedPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        response.executeTask(
                [response](const asyncgi::TaskContext& ctx) mutable
                {
                    auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                    timer->expires_after(std::chrono::seconds{3});
                    timer->async_wait([timer, response, ctx](auto&) mutable { // Note how we capture ctx object here,
                        response.send("Hello world"); // it's necessary to keep it (or its copy) alive
                    }); // before the end of request processing
                });
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::Get).process<DelayedPage>();
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}