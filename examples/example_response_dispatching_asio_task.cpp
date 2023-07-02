#include <asyncgi/asyncgi.h>
#include <asio/steady_timer.hpp>

namespace http = asyncgi::http;

struct DelayedPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto disp = asyncgi::AsioDispatcher{response};
        disp.postTask(
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
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process<DelayedPage>();
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = asyncgi::Server{io, router};
#ifndef _WIN32
    server.listen("/tmp/fcgi.sock");
#else
    server.listen("127.0.0.1", 9088);
#endif
    io.run();
    return 0;
}