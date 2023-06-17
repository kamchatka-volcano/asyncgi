#include <asyncgi/asyncgi.h>
#include <thread>

using namespace asyncgi;

struct DelayedPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        response.waitFuture(
                std::async(
                        std::launch::async,
                        []
                        {
                            std::this_thread::sleep_for(std::chrono::seconds(3));
                            return "world";
                        }),
                [response](const std::string& result) mutable
                {
                    response.send(http::Response{"Hello " + result});
                });
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    auto delayedPage = DelayedPage{};
    router.route("/", http::RequestMethod::Get).process(delayedPage);
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = app->makeServer(router);
#ifndef _WIN32
    server->listen("/tmp/fcgi.sock");
#else
    server->listen("127.0.0.1", 9088);
#endif
    app->exec();
    return 0;
}