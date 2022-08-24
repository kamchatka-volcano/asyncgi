#include <asyncgi/asyncgi.h>

using namespace std::string_literals;
struct DelayedPage : asyncgi::RequestProcessor<>{
    void process(const asyncgi::Request& request, asyncgi::Response<>& response) override
    {
        response.waitFuture(
            std::async(std::launch::async, []{std::this_thread::sleep_for(std::chrono::seconds(3)); return "World"s;}),
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
    router.route("/", http::RequestMethod::GET).process<DelayedPage>();
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}