#include <asyncgi/asyncgi.h>

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
                            return "World";
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
    router.route("/", http::RequestMethod::GET).process(delayedPage);
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}