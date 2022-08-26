#include <asyncgi/asyncgi.h>
#include <asio/steady_timer.hpp>

using namespace std::string_literals;
struct DelayedPage : asyncgi::RequestProcessor<>{
    void process(const asyncgi::Request&, asyncgi::Response<>& response) override
    {
        response.executeTask(
                [response](const asyncgi::TaskContext& ctx) mutable
                {
                    auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                    timer->expires_after(std::chrono::seconds{3});
                    timer->async_wait([timer, response, ctx](auto&) mutable{ //Note how we capture ctx object here,
                        response.send("Hello world"s);                       //it's necessary to keep it (or its copy) alive
                    });                                                      //before the end of request processing
                });
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::GET).process<DelayedPage>();
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}